

#include "Arduino.h"
#include "transport.h"
#include "protocol.h"
#include "messages.h"
#include "global_config.h"


// if(data_available){
//     // get the type and run the state machine look up function
// }

void rx_message_dispatcher(uint8_t type){ 

    switch(type){
    
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