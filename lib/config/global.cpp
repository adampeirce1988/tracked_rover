

#include <stdint.h>
#include <Arduino.h>
#include "global_config.h"
#include "protocol.h"
#include "global.h"
#include "debug.h"
#include "debug_config.h"

#define DEBUG_FILE DBG_SYSTEM 

/////////////////////////////////////////
//          SYSTEM GLOBALS             // 
/////////////////////////////////////////

namespace sys {

const char* verbous_current_state = "SAFE_STATE";

//test 
bool diagnostics_active = false; 

// system timestamp flags
uint32_t sys_heartbeat = 0; 
// comunication
uint32_t tx_last_valid_packet = 0; 
uint32_t last_connection_attempt = 0;
bool comunication_warn_active = false; // add for warning implimentation with auto reconnect. 
bool bus_connectivity_status = false; 
bool i2c_connectivity_status = true; // not yet actioned test only 
// Alive flags
//bool I2C_bus_alive = false; 
//bool Transport_alive = false; 
// error states 
bool I2C_bus_error = false; 
};

// function calls to update system variabls 

//update on a valid ack. 
void update_last_valid_comms(){
    sys::tx_last_valid_packet = millis();
    sys::bus_connectivity_status = true;  // this is never set false 
}

void update_last_connection_attempt(){
    sys::last_connection_attempt = millis();
    
}

void update_system_heartbeat(){
    sys::sys_heartbeat = millis();
}

void transport_alive_check(){
    if(sys::bus_connectivity_status == true){

        uint32_t time_since_last_packet = millis() - sys::tx_last_valid_packet; 

        if (time_since_last_packet  > COMUNICATION_IDLE_TIMEOUT){
            sys::bus_connectivity_status = false;
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "GLOB", "comunication timeout error no messages received for ", COMUNICATION_IDLE_TIMEOUT, "ms");
        }
        else if(time_since_last_packet > COMUNICATION_IDLE_WARN){
            if(sys::comunication_warn_active == false){
                establish_coms();   // send out a message establish coms to confirm comunication message is active. 
                sys::comunication_warn_active = true; 
                DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_WARN, "GLOB", "communication timeout warning no messages received for ", COMUNICATION_IDLE_WARN, "ms"); 
            }
        }
        else{
            //rest the comunication warn flag on sucsesfull transmisssion
            sys::comunication_warn_active = false;
        }
    }
}


// olny system alive checks to be added here. 
bool check_system_health_flags(){
    if(sys::bus_connectivity_status == true && sys::i2c_connectivity_status == true ){
        return true;
    }  
    else{
        return false;
    }
}
