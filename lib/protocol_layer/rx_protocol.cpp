
#include <stdint.h>
#include "transport.h"
#include "protocol.h"
#include "permissions.h"
#include "messages.h"


uint8_t mismatched_type_received; 
uint8_t last_packet_id_received; 

// frame for storing the read frame
frame protocol_frame; 


uint8_t rx_message_task_dispatcher(){ 

    transport_get_frame(&protocol_frame); // get the received frame and action.
    
    // check if a new packet is received
    if(last_packet_id_received == protocol_frame.ID){
        return PROTOCOL_RX_RETURN_CODE::PROTO_IDLE;
    }
    else{
        last_packet_id_received = protocol_frame.ID;
    }
    

    // check if this packet is inhibited 
    PERMISSION_RETURN_CODE result = is_message_allowed_in_current_state(protocol_frame.TYPE);

    if(result == PERMISSION_RETURN_CODE::MSG_VALID_INHIBITED){
        return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_MSG_INHIBITED;
    }
    else if(result == PERMISSION_RETURN_CODE::MSG_INVALID){
        return PROTOCOL_RX_RETURN_CODE::PROTO_INVALID_TYPE;
    }
    else if(result == PERMISSION_RETURN_CODE::MSG_VALID_ALLOWED){
        
        switch(protocol_frame.TYPE){

            case MSG_RESERVED_00: 
                    // default reserved type 
            break;  
        
            case MSG_ESTABLISH_COMMUNICATION:
                transport_queue_message(MSG_CONFIRM_COMMUNICATION, TRANSPORT_ACK_TYPE::NORMAL_FRAME, 0, NULL);

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