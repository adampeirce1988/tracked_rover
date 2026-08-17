#include <Arduino.h>

#include "system_internal.h"
#include "global_config.h"
#include "protocol.h"
#include "debug.h"

/*=============================================================================*
    * Debug Configuration
*=============================================================================*/

#define DEBUG_FILE DBG_SYSTEM_SERVICES


/*=============================================================================*
    * System Service functions
*=============================================================================*/

// System health and communication monitoring services.
bool check_system_health_flags(){
    return sys::bus_connectivity_status && 
           sys::i2c_connectivity_status;
}

void set_communication_bus_alive(){
    sys::bus_connectivity_status = true; 
}

// Called after a transmitted frame has been successfully acknowledged.
// Updates the last known valid communication timestamp and marks the bus active.
void update_last_valid_comms(){
    sys::tx_last_valid_packet = millis();
    sys::bus_connectivity_status = true;  // this is never set false change not yet implimented
}

void update_last_connection_attempt(){
    sys::last_connection_attempt = millis();
    
}

void update_system_heartbeat(){
    sys::sys_heartbeat = millis();
}

/*=============================================================================*
    * Communication Monitoring
*=============================================================================*/

void check_transport_alive(){
    if(sys::bus_connectivity_status){

        uint32_t time_since_last_packet = millis() - sys::tx_last_valid_packet; 

        if (time_since_last_packet  > COMMUNICATION_IDLE_TIMEOUT){
            sys::bus_connectivity_status = false;
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "SYS", "communication timeout error. No messages received for ", COMMUNICATION_IDLE_TIMEOUT, "ms");
        }
        else if(time_since_last_packet > COMMUNICATION_IDLE_WARN){
            if(!sys::communication_warn_active){
                establish_coms();                       // Attempt to re-establish communication before declaring the bus failed.
                sys::communication_warn_active = true; 
                DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_WARN, "SYS", "communication timeout warning. No messages received for ", COMMUNICATION_IDLE_WARN, "ms"); 
            }
        }
        else{
            // Reset the communication warn flag on successful communication
            sys::communication_warn_active = false;
        }
    }
}