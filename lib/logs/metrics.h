#ifndef METRICS_H
#define METRICS_H

#include<stdint.h>

// config variables
constexpr uint8_t LATENCY_BUFFER_SIZE = 100;   // maximum safe size for uint8_t indexing/counting is 255

// ring buffer struct
struct latency_buffer{
    uint16_t latency[LATENCY_BUFFER_SIZE];   // latency buffer
    uint8_t index = 0;                       // current index to be written next 
    uint8_t count = 0;                       // total entries in buffer 
    uint32_t sum = 0;                        // total sum of buffer
};

// initialise the latency ring buffers
extern latency_buffer rx_latency_buffer;
extern latency_buffer tx_latency_buffer;

// forward declerations 
void tx_latency_add_value(uint16_t latency);
void rx_latency_add_value(uint16_t latency);

// required internal call for internal module to call the get metrics functions
uint16_t latency_get_max(latency_buffer &b);
uint16_t latency_get_average(latency_buffer &b);
uint16_t latency_get_jitter(latency_buffer &b);
uint16_t latency_get_min(latency_buffer &b);

#endif