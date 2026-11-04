#ifndef TELEMETRY_SCHEDULER_H
#define TELEMETRY_SCHEDULER_H

#include "telemetry_types.h"

void telemetry_init();           // initiate all values and fetch avaliable data 
void run_telemetry_scheduler();  //runs the scheduler and action any telemetry updates 
     
#endif