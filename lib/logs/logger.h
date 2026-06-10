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
//bool st_check_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type,  uint8_t expected_result); // types not exposed
void rt_erase_error_codes(); 


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