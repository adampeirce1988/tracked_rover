#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "transport.h" 
#include "debug.h"
#include "debug_config.h"

// function ptototypes
void uart_write(uint8_t byte);
uint8_t uart_read();
uint8_t uart_available();

//uart struct instance
Transport_IO uart_io = {
    uart_write,
    uart_available,
    uart_read,
    uart_begin
};



#endif