#pragma once

class MotorControl {
public:
  void begin();
  void setSpeed(int leftSpeed, int rightSpeed);

private:
  void configureMotorPins();
  void writeMotor(int pwmPin, int dirPin, int speed);
};
