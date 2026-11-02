#include <stdint.h>
#include <Arduino.h>
#include "debug.h"
#include "json.h"
#include "telemetry_internal.h"
#include "telemetry_types.h"

// configure frequancy of timers
constexpr uint16_t HIGH_SPEED_INTERVAL_MS = 100; 
constexpr uint16_t LOW_SPEED_INTERVAL_MS = 250; 

// variables for storing the last update time 
uint32_t high_speed_timer = 0;
uint32_t low_speed_timmer = 0;

void run_telemetry_scheduler(){

    uint32_t cache_millis = millis();

    // run high speed telemetry 
    if(cache_millis - high_speed_timer > HIGH_SPEED_INTERVAL_MS)
    {
        
        high_speed_telemetry_fetch_data();

        const telemetry_data_t& data = telemetry_get_data(); 

        String high_speed_data = generate_telemetry_json_string(data);

        // update the timer 
        high_speed_timer = cache_millis;

        // temp solution before linking the json to the websocket
        DEBUG_PORT.println(high_speed_data); 
    }

    // run low speed telemetry
    if(cache_millis - low_speed_timmer > LOW_SPEED_INTERVAL_MS){

        low_speed_telemetry_fetch_data(); 


        // core functions go here 
        low_speed_timmer = cache_millis; 
    }

}