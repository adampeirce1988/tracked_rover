
#ifndef FIFO_H
#define FIFO_H

#include <Arduino.h>
#include "transport.h" 
#include "debug.h"
#include "debug_config.h"

// function ptototypes
void fifo_write(uint8_t byte);
uint8_t fifo_read();
uint8_t fifo_available();

//uart struct instance
Transport_IO fifo_io = {
    fifo_write,
    fifo_available,
    fifo_read,
    fifo_begin
};

#endif