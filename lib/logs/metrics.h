#ifndef METRICS_H
#define METRICS_H

#include<stdint.h>

// config variables
constexpr uint8_t LATENCY_BUFFER_SIZE = 100;   // maximum safe size for uint8_t indexing/counting is 255

// forward declerations 
void tx_latency_add_value(uint16_t latency);
void rx_latency_add_value(uint16_t latency);

#endif