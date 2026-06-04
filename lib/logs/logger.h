#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

// -----------------------------
// Logging API
// -----------------------------

// event log router
void process_transport_rx_return_error(uint8_t return_code);
void process_transport_tx_return_error(uint8_t return_code);
void process_protocol_return_error(uint8_t return_code); // not yet implimented 

void st_clear_log();
void rt_clear_log(); // not yet implimented.

// metrics API's
// TX API function prototypes 
void tx_latency_clear_buffer();
uint16_t tx_latency_get_min();
uint16_t tx_latency_get_max();
uint16_t tx_latency_get_average();
uint16_t tx_latency_get_jitter();

// RX API function prototypes 
void rx_latency_clear_buffer();
uint16_t rx_latency_get_min();
uint16_t rx_latency_get_max();
uint16_t rx_latency_get_average();
uint16_t rx_latency_get_jitter();


#endif