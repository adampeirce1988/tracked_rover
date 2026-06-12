#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

enum ST_TEST_ENTRY {
    ST_LOG_PACKETS_SENT,
    ST_LOG_ACKS_TRANSMITTED,
    ST_LOG_RETRY_ATTEMPT,
    ST_LOG_ACK_NOT_RECEIVED, 
    ST_LOG_ACK_MISMATCH, 
    ST_LOG_ACK_TIMEOUT,
    ST_LOG_TX_BUFFER_OVERFLOW, 
    ST_LOG_PACKETES_RECEIVED,
    ST_LOG_ACK_RECEIVED, 
    ST_LOG_NACK_RECEIVED, 
    ST_LOG_INVALID_TPYE, 
    ST_LOG_ACK_OUT_OF_RANGE, 
    ST_LOG_DLC_OVER_CAPACITY,
    ST_LOG_CRC_ERRORS,
    ST_LOG_MSG_TIMEOUT,
    ST_LOG_TOTAL_ERRORS
};

enum EVALUATION_TYPE{
    EQUAL,
    GREATER_THAN,
    LESS_THAN
};

// -----------------------------
// Logging API
// -----------------------------

// event log router
void process_transport_rx_return_error(uint8_t return_code);
void process_transport_tx_return_error(uint8_t return_code);
void process_protocol_return_error(uint8_t return_code); // not yet implimented 

void st_clear_log();
void st_print_log();

bool st_check_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type, uint8_t expected_result); // types not exposed
bool st_compare_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type, ST_TEST_ENTRY expected_result);
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