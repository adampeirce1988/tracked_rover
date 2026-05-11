

#include <stdint.h>
#include <Arduino.h>
#include "global_config.h"
#include "global.h"
#include "debug.h"
#include "debug_config.h"

#define DEBUG_FILE DBG_SYSTEM 

/////////////////////////////////////////
//          SYSTEM GLOBALS             // 
/////////////////////////////////////////

namespace sys {

const char* verbous_current_state = "SAFE_STATE";

// system timestamp flags
uint32_t sys_heartbeat = 0; 
// comunication
uint32_t tx_last_valid_packet = 0; 
uint32_t last_connection_attempt = 0;
bool connection_established = false; 
// Alive flags
bool I2C_bus_alive = false; 
bool Transport_alive = false; 
// error states 
bool I2C_bus_error = false; 
};

// function calls to update system variabls 

//update on a valid ack. 
void update_last_valid_comms(){
    sys::tx_last_valid_packet = millis();
    sys::connection_established = true;  // this is never set false 
}

void update_last_connection_attempt(){
    sys::last_connection_attempt = millis();
    
}

void update_system_heartbeat(){
    sys::sys_heartbeat = millis();
}


bool check_system_heartbeat_timeout(){
    if(sys::sys_heartbeat - millis() > SYSTEM_NOT_ALIVE_TIMEOUT){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "SYS", "System heartbeat timed out entering safe_state");
        return true; 
    }
    else{
        return false;
    }
}

