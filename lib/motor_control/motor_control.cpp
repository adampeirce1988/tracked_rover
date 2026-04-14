#include <Arduino.h>
#include "motor_control.h"
#include "config.h"

void MotorControl::begin() {
  configureMotorPins();
}

void MotorControl::setSpeed(int leftSpeed, int rightSpeed) {
  writeMotor(LEFT_MOTOR_PWM_PIN, LEFT_MOTOR_DIR_PIN, leftSpeed);
  writeMotor(RIGHT_MOTOR_PWM_PIN, RIGHT_MOTOR_DIR_PIN, rightSpeed);
}

void MotorControl::configureMotorPins() {
  pinMode(LEFT_MOTOR_PWM_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_DIR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR_PIN, OUTPUT);
}

void MotorControl::writeMotor(int pwmPin, int dirPin, int speed) {
  bool direction = speed >= 0;
  int pwmValue = constrain(abs(speed), 0, 255);
  digitalWrite(dirPin, direction ? HIGH : LOW);
  analogWrite(pwmPin, pwmValue);
}
