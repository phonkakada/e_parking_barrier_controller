#include "ParkingBarrierController.h"
#include <WiFi.h>


// Outside servo = 22 , ir = 0
// Inside servo = 23 , ir = 4
ParkingBarrierController parkingBarrierController("message" , 0 , 4);

const char* ssid = "Hello";
const char* password = "c7A6y5NFL3f81JKK";




void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("\nConnected to ");
  Serial.println(WiFi.localIP());
  parkingBarrierController.connectToWebSocket();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    parkingBarrierController.run();
  }

}
