#pragma once

#include <Arduino.h>

class Rover {
public:
  void begin();
  void update();

private:
  void readSensors();
  void driveMotors();
  int calculateSpeed();

  int targetSpeed = 0;
};
