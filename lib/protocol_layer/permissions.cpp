#include <Arduino.h>
#include "global_config.h"
#include "permissions.h"
#include "protocol.h"
#include "system.h"
#include "system_internal.h"
#include "debug.h"

#define DEBUG_FILE DBG_PROTOCOL


PERMISSION_RETURN_CODE is_message_allowed_in_current_state(uint8_t type){

    uint8_t state_mask = (1U << static_cast<uint8_t>(get_active_vehicle_state())); // change to mask based on vehicle state. 

    for(size_t index = 0; index < sizeof(rules) / sizeof(rules[0]); index++){
        if(type == rules[index].msg){
            if(rules[index].bit_mask & state_mask){ 
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "PREM", "message valid in current vehicle state type: ", type );
                return PERMISSION_RETURN_CODE::MSG_VALID_ALLOWED; 
            }
            else{
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "PREM", "message not valid in current vehicle state. type: ", type );
                return PERMISSION_RETURN_CODE::MSG_VALID_INHIBITED;
            }
        } 
    }
    return PERMISSION_RETURN_CODE::MSG_INVALID; 
}

