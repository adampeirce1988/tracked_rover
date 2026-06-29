#include "messages.h"
#include "global_config.h"
#include "transport.h"


void establish_coms(){
    transport_queue_message(MSG_ESTABLISH_COMMUNICATION, TRANSPORT_ACK_TYPE::ACK_REQUEST, 0, nullptr);
}

void confirm_coms(){
    transport_queue_message(MSG_CONFIRM_COMMUNICATION, TRANSPORT_ACK_TYPE::NORMAL_FRAME, 0, nullptr);
}

