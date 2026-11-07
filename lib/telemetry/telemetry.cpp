
#include <Arduino.h> // remove once hs data is added. 

#include <stdint.h> 
#include "telemetry_types.h"
#include "telemetry_internal.h"
#include "network.h"
#include "test.h"

// store telemetry data
static high_speed_telemetry_data_t hs_telemetry_data; 
static low_speed_telemetry_data_t ls_telemetry_data; 


void telemetry_init(){ 

    // Initialise telemetry values
    ls_telemetry_data.vehicle_state = "BOOTING";
    ls_telemetry_data.battery_voltage = 0.0f;
    ls_telemetry_data.wifi_mode = "UNKNOWN";
    ls_telemetry_data.wifi_signal = 0;

    hs_telemetry_data.acc_x = 0;
    hs_telemetry_data.acc_y = 0;
    hs_telemetry_data.acc_z = 0;
    hs_telemetry_data.gyro_x = 0;
    hs_telemetry_data.gyro_y = 0;
    hs_telemetry_data.gyro_z = 0;

    // Populate available telemetry
    low_speed_telemetry_fetch_data();
    high_speed_telemetry_fetch_data();
    
}

// fetch all telemetry data that requires updating at 4Hz (250ms)
void low_speed_telemetry_fetch_data (){

    ls_telemetry_data.vehicle_state = get_active_state_as_string();   // change to get vehicle state function 
    ls_telemetry_data.battery_voltage = get_test_battery_voltage();   // fetch test battey data 
    ls_telemetry_data.wifi_mode = wifi_current_mode();                // get the current wifi connection type 
    ls_telemetry_data.wifi_signal = wifi_signal_strength();           // get wifi signal strength 
 
}

// fetch telemetry data at 10Hz (100ms)
void high_speed_telemetry_fetch_data(){
    // sensor telemtry gos here. 
    hs_telemetry_data.acc_x = random(5, 50);
    hs_telemetry_data.acc_y = random(5, 50);
    hs_telemetry_data.acc_z = random(5, 50);
    hs_telemetry_data.gyro_x = random(5, 50);
    hs_telemetry_data.gyro_y = random(5, 50);
    hs_telemetry_data.gyro_z = random(5, 50);
    
}


// return stored telemetry data as a constant
const low_speed_telemetry_data_t& low_speed_telemetry_get_data(){
    return ls_telemetry_data; 

}

const high_speed_telemetry_data_t& high_speed_telemetry_get_data(){
    return hs_telemetry_data;
}