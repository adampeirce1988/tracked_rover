
#ifndef FIFO_H
#define FIFO_H

#include <Arduino.h>
#include "transport.h" 
#include "debug.h"
#include "debug_config.h"

#define MAX_FRAME_SIZE(f) (sizeof (struct frame))
#define DYNAMIC_FRAME_SIZE(f) (sizeof(struct frame) - MAX_PAYLOAD_LEN + (f).DLC)

// function ptototypes
// void fifo_write(uint8_t byte);
// uint8_t fifo_read();
// uint8_t fifo_available();
// void fifo_begin(uint32_t baud_rate);

extern Transport_IO fifo_io; 

#endif