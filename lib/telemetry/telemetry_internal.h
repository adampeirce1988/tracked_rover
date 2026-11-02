#ifndef TELEMETRY_H
#define TELEMETRY_H 

#include <stdint.h>
#include "telemetry_types.h"

void low_speed_telemetry_fetch_data();          // update the low speed telemetry 
void high_speed_telemetry_fetch_data();         // update the high speed telemetry           
const telemetry_data_t& telemetry_get_data();   // retruns a constant copy of the current telemetry data

#endif