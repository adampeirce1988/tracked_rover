#ifndef CONFIG_H
#define CONFIG_H    

// Configuration constants for the tracked rover.

#define ROVER_LOOP_DELAY_MS 10

// Motor pins for Arduino UNO
#define LEFT_MOTOR_PWM_PIN 5
#define LEFT_MOTOR_DIR_PIN 4
#define RIGHT_MOTOR_PWM_PIN 6
#define RIGHT_MOTOR_DIR_PIN 7

// Motor pins for ESP32
#ifdef ROVER_PLATFORM_ESP32
  #define LEFT_MOTOR_PWM_PIN 18
  #define LEFT_MOTOR_DIR_PIN 19
  #define RIGHT_MOTOR_PWM_PIN 21
  #define RIGHT_MOTOR_DIR_PIN 22
#endif


#endif // CONFIG_H