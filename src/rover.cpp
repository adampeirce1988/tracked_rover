#include "rover.h"
#include "config.h"
#include "motor_control.h"

static MotorControl motors;

void Rover::begin() {
  Serial.println("Initializing rover...");
  motors.begin();
}

void Rover::update() {
  readSensors();
  driveMotors();
}

void Rover::readSensors() {
  // TODO: add sensor input here (e.g. line sensors, ultrasonic, encoders)
  targetSpeed = calculateSpeed();
}

void Rover::driveMotors() {
  motors.setSpeed(targetSpeed, targetSpeed);
}

int Rover::calculateSpeed() {
  // TODO: replace with real control logic.
  return 150;
}
