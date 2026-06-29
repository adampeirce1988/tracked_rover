#ifndef METRICS_H
#define METRICS_H

#include<stdint.h>

/////////////////////////////////////////////////
//               config variables              //
/////////////////////////////////////////////////

constexpr uint16_t LATENCY_BUFFER_SIZE = 100;  

/////////////////////////////////////////////////
//            structs & declarations           //
/////////////////////////////////////////////////

// ring buffer struct
struct latency_buffer{
    uint16_t samples[LATENCY_BUFFER_SIZE] = {};   // latency buffer
    uint16_t index = 0;                           // current index to be written next 
    uint16_t count = 0;                           // total entries in buffer 
    uint32_t sum = 0;                             // total sum of buffer
};

/////////////////////////////////////////////////
//            forward declarations             //
/////////////////////////////////////////////////

void tx_latency_add_value(uint16_t latency);
void rx_latency_add_value(uint16_t latency);

#endif