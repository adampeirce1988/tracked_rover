
#include <Arduino.h>
#include "transport_fifo.h"
#include "transport.h"
#include "global_config.h"
#include "debug.h"
#include "debug_config.h"

#define DEBUG_FILE DBG_TRANSPORT_FIFO

void fifo_write(uint8_t byte){
    COMS_PORT.write(byte);
}

uint8_t fifo_read(){
    return COMS_PORT.read();
}

uint8_t fifo_available(){
    return COMS_PORT.available();
}

extern void fifo_begin(uint32_t baud_rate){
    COMS_PORT.begin(baud_rate);
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "communication port started at: ", baud_rate);
}