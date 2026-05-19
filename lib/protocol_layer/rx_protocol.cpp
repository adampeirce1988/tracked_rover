

#include "Arduino.h"
#include "transport.h"
#include "protocol.h"
#include "messages.h"
#include "global_config.h"

uint8_t missmatche_type_received; 

frame protocol_fame; 

uint8_t rx_message_task_dispatcher(){ 

    transport_get_frame(&protocol_fame); // get the rceived frame and action.
    uint8_t type = protocol_fame.TYPE;

    switch(type){

        case MSG_RESERVED_00: 
           return PROTOCOL_RX_RETURN_CODE::PROTO_RESERVED_TYPE; // default reserved type 
        break;  
    
        case MSG_ESTABLISH_COMMUNICATION:
            transport_queue_message(MSG_CONFIRM_COMMUNICATION, NORMAL_FRAME, 0, NULL);
            return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE;
        break;

        case MSG_CONFIRM_COMMUNICATION: 
            //uart_connection_established == true; 
            // update confirmed comunicatiob move the var to global.
            return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE; 
        break; 

        case MSG_EMERGENCY_STOP:
            // request safe state. 
            return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE; 
        break;

        case MSG_SET_MOTOR_SPEED: 
            // if(my_case == idle || mycase == running){
            //     //run the function
            //     return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE;
            // }
            // else if(mycase != idle || mycase != running){
            //     return PROTOCOL_RX_RETURN_CODE::PROTO_VALID_MSG_INHIBITED;
            // }
                
        default:
            missmatche_type_received = type; 
            return PROTOCOL_RX_RETURN_CODE::PROTO_INVALID_TYPE; // invalid message received. log an error here. 
    }


}