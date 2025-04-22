#include <ESP32Servo.h>

class ServoController {
  private:
    Servo servo;
    int servoPin;
    int currentDegree;
  public:
    ServoController(int servoPin) {
      this -> servoPin = servoPin;
      servo.attach(this -> servoPin);
      servo.write(0);
    }
    void openBarrier() {
      for (currentDegree = 0; currentDegree <= 90; currentDegree += 1) {
        servo.write(currentDegree);
        delay(15);
      }
    }
    void closeBarrier() {
      for (currentDegree = currentDegree; currentDegree >= 0; currentDegree -= 1) {
        servo.write(currentDegree);
        delay(15);
      }
    }
  
};