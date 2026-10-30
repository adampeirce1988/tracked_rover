
#include <stdint.h>
#include <Arduino.h>
#include "debug.h"
#include "telemetry_types.h"
#include "telemetry.h"
#include "json.h"


//Test only variables
float test_battery_volatge = 21.22;   // variable to hold the test value
uint32_t last_update_interval = 0;    // variable to hold last update 
uint16_t update_frequancy = 250;      // frequancy of update 

extern void update_test_battery_voltage(){ 

    if(millis() - last_update_interval > update_frequancy){ 

        // update yhe next interval  
        last_update_interval = millis();

        uint8_t random_value = random(0,100);

        if(random_value < 50){ 
            test_battery_volatge += 0.01; 
        }
        else{
            test_battery_volatge -= 0.01; 
        }

        if(test_battery_volatge > 23.00){
            test_battery_volatge -= 0.02; 
        }
        else if(test_battery_volatge < 21.00){
            test_battery_volatge+= 0.02;
        }
        
        // print to the console for testing only 
        //DEBUG_PORT.print("test_battery_voltage: "); 
        //DEBUG_PORT.print(test_battery_volatge);
        //DEBUG_PORT.println(" v");
    }
}


extern float get_test_battery_voltage(){
    return test_battery_volatge; 
}


void  json_test(){

    telemetry_fetch_data();

    const telemetry_data_t& data = telemetry_get_data();

    String json_test = generate_telemetry_json_string(data); 

    DEBUG_PORT.println(json_test); 

}