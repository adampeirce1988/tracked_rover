

#include <stdint.h> 
#include "telemetry_types.h"
#include "network.h"
#include "test.h"

// store telemetry data
static telemetry_data_t telemetry_data; 


void telemetry_init(){ 

    // Initialise telemetry values
    telemetry_data.vehicle_state = "BOOTING";
    telemetry_data.battery_voltage = 0.0f;
    telemetry_data.wifi_mode = "UNKNOWN";
    telemetry_data.wifi_signal = 0;

    // Populate available telemetry
    low_speed_telemetry_fetch_data();
    high_speed_telemetry_fetch_data();
    
}

// fetch all telemetry data that requires updating at 4Hz (250ms)
void low_speed_telemetry_fetch_data (){

    telemetry_data.vehicle_state = get_active_state_as_string();  // change to get vehicle state function 
    telemetry_data.battery_voltage = get_test_battery_voltage();  // fetch test battey data 
    telemetry_data.wifi_mode = wifi_current_mode();               // get the current wifi connection type 
    telemetry_data.wifi_signal= wifi_signal_strength();           // get wifi signal strength 

}

// fetch telemetry data at 10Hz (100ms)
void high_speed_telemetry_fetch_data(){
    // sensor telemtry gos here. 
}


// return stored telemetry data as a constant
const telemetry_data_t& telemetry_get_data(){
    return telemetry_data; 

}