
#include <stdint.h>
#include "transport.h"
#include "protocol.h"
#include "permissions.h"
#include "messages.h"
#include "debug.h"
#include "system.h"

#define DEBUG_FILE DBG_PROTOCOL

uint8_t mismatched_type_received; 
uint8_t last_packet_id_received; 

// frame for storing the read frame
frame protocol_frame; 


uint8_t rx_message_task_dispatcher(){ 

    
    // return if no frames are avaliable
    if(frame_avaliable() == 0){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "PROTO", "NO frames avaliable");
        return PROTOCOL_RX_RETURN_CODE::PROTO_IDLE;
    }
    

    // get the received frame and clear rx_packet.frame_ready
    transport_get_frame(&protocol_frame); 
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "PROTO", "frame copied from Transport to protocol");

    // check if this packet is inhibited 
    PERMISSION_RETURN_CODE result = is_message_allowed_in_current_state(protocol_frame.TYPE);

    if(result == PERMISSION_RETURN_CODE::MSG_VALID_INHIBITED){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "PROTO", "message status: MSG_VALID_INHIBITED");
        return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_MSG_INHIBITED;
    }
    else if(result == PERMISSION_RETURN_CODE::MSG_INVALID){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "PROTO", "message status: MSG_INVALID");
        return PROTOCOL_RX_RETURN_CODE::PROTO_INVALID_TYPE;
    }
    else if(result == PERMISSION_RETURN_CODE::MSG_VALID_ALLOWED){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "PROTO", "message status: MSG_VALID_ALLOWED");

        
        switch(protocol_frame.TYPE){

            case MSG_RESERVED_00: 
                    // default reserved type 
            break;  
        
            case MSG_ESTABLISH_COMMUNICATION:
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "PROTO", "Running: MSG_ESTABLISH_COMMUNICATION");
                set_comunication_bus_alive(); 
            break;

            case MSG_CONFIRM_COMMUNICATION: 
                //uart_connection_established == true; 
                // update confirmed communication; move the var to global.
            break; 

            case MSG_EMERGENCY_STOP:
                // request safe state. 
            break;

            case MSG_SET_MOTOR_SPEED: 
                // if(my_case == idle || mycase == running){
                //     //run the function
                //     return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE;
                // }
                // else if(mycase != idle || mycase != running){
                //     return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_MSG_INHIBITED;
                // }
            break; 
            
            case MSG_TEST_INVALID:
                // code goes here 
            break;

            case MSG_TEST_INHIBITED:
                // code goes here 
            break;

            case MSG_TEST_RESERVED:
                // code goes here
            break;

            case MSG_TEST_VALID: 
                // code goes here 
            break;

            // default return value. 
            default:
                mismatched_type_received = protocol_frame.TYPE; 
                return PROTOCOL_RX_RETURN_CODE::PROTO_INVALID_TYPE; // invalid message received. log an error here. 
        }

    }
    
    return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE;
}