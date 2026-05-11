#include "messages.h"
#include "global_config.h"
#include "transport.h"


void establish_coms(){
    transport_queue_message(MSG_ESTABLISH_COMMUNICATION, ACK_REQUEST, 0, NULL);
}


void confirm_coms(){
    transport_queue_message(MSG_CONFIRM_COMMUNICATION, NORMAL_FRAME, 0, NULL);
}

