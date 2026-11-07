#ifndef TELEMETRY_TYPES_H
#define TELEMETRY_TYPES_H

#include <stdint.h> 
#include "system.h"

struct low_speed_telemetry_data_t {
    const char* vehicle_state = "UNKNOWN";
    float battery_voltage = 0.00f; 
    int8_t wifi_signal = 0; 
    const char* wifi_mode = "---";  
};

struct high_speed_telemetry_data_t {

    // accelerometer 
    uint16_t acc_x = 0; 
    uint16_t acc_y = 0;
    uint16_t acc_z = 0;
    // gyroscope 
    uint16_t gyro_x = 0;
    uint16_t gyro_y = 0;
    uint16_t gyro_z = 0;


};


#endif