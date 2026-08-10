#include <stdint.h>
#include "system_internal.h"
#include "global_config.h"
#include "protocol.h"
#include "debug.h"
#include "debug_config.h"

#define DEBUG_FILE DBG_SYSTEM_SERVICES

// only system alive checks to be added here. 
bool check_system_health_flags(){
    if(sys::bus_connectivity_status && 
        sys::i2c_connectivity_status ){
        return true;
    }  
    else{
        return false;
    }
}

void set_communication_bus_alive(){
    sys::bus_connectivity_status = true; 
}

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

void check_transport_alive(){
    if(sys::bus_connectivity_status == true){

        uint32_t time_since_last_packet = millis() - sys::tx_last_valid_packet; 

        if (time_since_last_packet  > COMMUNICATION_IDLE_TIMEOUT){
            sys::bus_connectivity_status = false;
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "GLOB", "communication timeout error no messages received for ", COMMUNICATION_IDLE_TIMEOUT, "ms");
        }
        else if(time_since_last_packet > COMMUNICATION_IDLE_WARN){
            if(sys::communication_warn_active == false){
                establish_coms();   // send out a message establish coms to confirm communication message is active. 
                sys::communication_warn_active = true; 
                DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_WARN, "GLOB", "communication timeout warning no messages received for ", COMMUNICATION_IDLE_WARN, "ms"); 
            }
        }
        else{
            //rest the communication warn flag on sucsesful transmisssion
            sys::communication_warn_active = false;
        }
    }
}