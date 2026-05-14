

#include "Arduino.h"
#include "transport.h"
#include "protocol.h"
#include "messages.h"
#include "global_config.h"

struct frame protocol_frame; 


void rx_message_dispatcher(struct frame *f){ 

    uint8_t type; 

    if(frame_avaliable() > 0){
        transport_get_frame(&protocol_frame);
        type = f->TYPE; 
    }
    else{
        type = MSG_RESERVED_00;
    }

    switch(type){

        case MSG_RESERVED_00: 
           return; // default reserved type 
        break;  
    
        case MSG_ESTABLISH_COMMUNICATION:
            transport_queue_message(MSG_CONFIRM_COMMUNICATION, NORMAL_FRAME, 0, NULL);
        break;

        case MSG_CONFIRM_COMMUNICATION: 
            //uart_connection_established == true; 
            // update confirmed comunicatiob move the var to global.
        break; 

        case MSG_COMMUNICATION_ERROR:
            // communication error
        break;

    }


}