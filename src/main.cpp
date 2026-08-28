
// #include "system.h"
// #include "debug.h"

// void setup(){

//       debug_port_begin();                                                    // open the debug port
//       delay(1000);                                                           // run 1s delay before transmitting data 
//       PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES);       // print version and metadata 
//       delay(1000);
//  // All setup function should be completed in vehicle state booting
//  // this state is the default state on power up

// }

// void loop() {

//   run_vehicle_state();                                           // run vehicle requested FSM. 

// }
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);

    delay(2000);

    Serial.println("SETUP START");
    Serial.println("SETUP RUNNING");
}

void loop()
{
    Serial.println("LOOP");
    delay(1000);
}