#ifndef TELEMETRY_TYPES_H
#define TELEMETRY_TYPES_H

#include <stdint.h> 
#include "system.h"

struct telemetry_data_t {
    const char* vehicle_state = "UNKNOWN";
    float battery_voltage = 0.00f; 
};


#endif