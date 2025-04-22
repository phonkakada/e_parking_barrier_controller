#include "esp32-hal-gpio.h"
#include "HardwareSerial.h"
#include "WString.h"
#include "esp32-hal.h"
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include "ServoController.h"
using namespace websockets;

ServoController inSideServoController(23);
ServoController outSideServoController(22);

class ParkingBarrierController {
private:
  const int ledPin = 2;  // GPIO pin for LED
  unsigned long previousMillis = 0;
  const long interval = 500;  // Blink interval in milliseconds
  bool ledState = false;
  WebsocketsClient client;
  const char* websocket_server = "ws://188.166.206.171:6001/app/zmdwgehgzb3pkerywvmx";
  String channelName;

  int irSensorPinInSide;
  int irSensorPinOutSide;


public:
  ParkingBarrierController(String channelName, int irSensorPinInSide, int irSensorPinOutSide) {
    this->channelName = channelName;
    this->irSensorPinInSide = irSensorPinInSide;
    this->irSensorPinOutSide = irSensorPinOutSide;
    pinMode(this-> irSensorPinInSide, INPUT);
    pinMode(this->irSensorPinOutSide, INPUT);
  }
  void blinkLed(){
  unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        ledState = !ledState;  // Toggle LED state
        digitalWrite(ledPin, ledState);
    }

}
  static void onMessageCallback(WebsocketsMessage message) {
    String parkingServiceId = "3310fced-3d42-4f32-be00-a7ed32df397a";  // define your service id
    // Step 1: Parse main JSON object
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, message.data());

    if (error) {
      Serial.print("❌ JSON parsing failed: ");
      Serial.println(error.f_str());
      return;
    }

    // Step 2: Extract the "data" field (which is itself a JSON string)
    const char* dataStr = doc["data"];

    // Step 3: Parse the nested JSON from the "data" string
    StaticJsonDocument<1024> dataDoc;
    DeserializationError nestedError = deserializeJson(dataDoc, dataStr);

    if (nestedError) {
      Serial.print("❌ Nested JSON parsing failed: ");
      Serial.println(nestedError.f_str());
      return;
    }

    // Step 4: Extract "barrier" and "status"
    JsonObject barrier = dataDoc["message"]["barrier"];
    const char* status = dataDoc["message"]["status"];

    // Step 5: Print extracted values
    Serial.println(" Extracted Data:");
    Serial.print("Barrier ID: ");
    Serial.println(barrier["id"].as<const char*>());

    Serial.print("Barrier Side: ");
    Serial.println(barrier["barrier_side"].as<const char*>());

    Serial.print("Status: ");
    Serial.println(status);

    String barrierSide = String(barrier["barrier_side"].as<const char*>());
    String barrierServiceId = String(barrier["parking_service_id"].as<const char*>());
    String barrierStatus = String(status);

    Serial.print("Parking Service Id: ");
    Serial.println(barrierServiceId + parkingServiceId);

    // if (parkingServiceId == barrierServiceId) {
    if (barrierSide == "InSide") {
      if (barrierStatus == "Open") {
        inSideServoController.openBarrier();
      } else if (barrierStatus == "Close") {
        inSideServoController.closeBarrier();
      }
    }

    if (barrierSide == "OutSide") {
      if (barrierStatus == "Open") {
        outSideServoController.openBarrier();
      } else if (barrierStatus == "Close") {
        outSideServoController.closeBarrier();
      }
    }
    // }
  }
  void connectToWebSocket() {
    if (client.connect(websocket_server)) {
      Serial.println("Connected to Laravel Reverb WebSocket!");
      String subscribeMessage = "{\"event\":\"pusher:subscribe\",\"data\":{\"channel\":\"barrier\"}}";
      client.send(subscribeMessage);
    } else {
      Serial.println("Failed to connect to WebSocket!");
    }
    client.onMessage(onMessageCallback);
  }
  void handleIRSensor() {
    if (digitalRead(irSensorPinOutSide) == 0) {
      delay(5000);
      outSideServoController.closeBarrier();
    }
    if (digitalRead(irSensorPinInSide) == 0) {
      delay(5000);
      inSideServoController.closeBarrier();
    }
  }
  void run() {
    if (client.available()) {
      digitalWrite(ledPin, 1);
      handleIRSensor();
      client.poll();
    } else {
      blinkLed();
      connectToWebSocket();
    }
  }
};