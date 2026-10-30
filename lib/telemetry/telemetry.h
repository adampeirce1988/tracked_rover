#ifndef TELEMETRY_H
#define TELEMETRY_H 

#include <stdint.h>
#include "telemetry_types.h"

void telemetry_init();                          // initilise the telemetry 
void telemetry_fetch_data();                    // update the telemetry 
const telemetry_data_t& telemetry_get_data();    // retruns a constant copy of the current telemetry data

#endif