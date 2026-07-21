
// TODO: on return to safe_state the paramer sys::comunication esatbolished to false on error. 

#include <Arduino.h>
#include <HardwareSerial.h>
#include "self_test.h"
#include "logger.h"
#include "transport.h"
#include "debug.h"
#include "protocol.h"
#include "global_config.h"
#include "global.h"
#include "messages.h"
#include "system.h"

#define DEBUG_FILE DBG_ESP_MAIN

// return code variable ** these are also stored in system.h** 
uint8_t rx_transport_status = 0;  
uint8_t tx_transport_status = 0;
uint8_t rx_protocol_status = 0; 
SELF_TEST_MANAGER_RETURN_CODE self_testmanager_status = SELF_TEST_MANAGER_RETURN_CODE::ERROR;

void setup(){
 // All setup function should be completed in vehicle state booting
 // this state is the default state on power up

}

void loop() {

  run_vehicle_state();                                           // run vehicle requested FSM. 

}
