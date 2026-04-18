
#include <Arduino.h>
#include "transport_fifo.h"
#include "transport.h"
#include "global_config.h"
#include "debug.h"
#include "debug_config.h"

#define DEBUG_FILE DBG_TRANSPORT_UART

// Forward declarations
void uart_write(uint8_t byte);
uint8_t uart_read();
uint8_t uart_available();
void uart_begin(uint32_t baud_rate);

//uart struct instance
Transport_IO uart_io = {
    uart_write,
    uart_available,
    uart_read,
    uart_begin
};

void uart_write(uint8_t byte){
    COMS_PORT.write(byte);
}

uint8_t uart_read(){
    return COMS_PORT.read();
}

uint8_t uart_available(){
    return COMS_PORT.available();
}

extern void uart_begin(uint32_t baud_rate){
    COMS_PORT.begin(baud_rate);
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "communication port started at: ", baud_rate);
}