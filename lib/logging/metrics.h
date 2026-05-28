#pragma once 

#include<stdint.h>

// config variables
constexpr uint8_t LATENCY_BUFFER_SIZE = 100;   // maximum safe size for uint8_t indexing/counting is 255

// TX API function prototypes 
void tx_latency_add_value(uint16_t latency);
void tx_latency_clear_buffer();
uint16_t tx_latency_get_min();
uint16_t tx_latency_get_max();
uint16_t tx_latency_get_average();
uint16_t tx_latency_get_jitter();

// RX API function prototypes 
void rx_latency_add_value(uint16_t latency);
void rx_latency_clear_buffer();
uint16_t rx_latency_get_min();
uint16_t rx_latency_get_max();
uint16_t rx_latency_get_average();
uint16_t rx_latency_get_jitter();
