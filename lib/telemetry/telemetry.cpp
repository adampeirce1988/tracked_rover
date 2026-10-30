

#include <stdint.h> 
#include "telemetry_types.h"
#include "test.h"

// store telemetry data
static telemetry_data_t telemetry_data; 


void telemetry_init(){ 
    // telemetry initilisation code goes here
}

// fetch all telemetry data
void telemetry_fetch_data (){

    telemetry_data.vehicle_state = get_active_state_as_string();  // change to get vehicle state function 
    telemetry_data.battery_voltage = get_test_battery_voltage();  // fetch test battey data 

}

// return stored telemetry data as a constant
const telemetry_data_t& telemetry_get_data(){
    return telemetry_data; 

}