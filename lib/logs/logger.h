#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include "transport.h"

enum ST_TEST_ENTRY : uint8_t {
    PACKETS_SENT,
    ACKS_TRANSMITTED,
    RETRY_ATTEMPT,
    ACK_NOT_RECEIVED, 
    ACK_MISMATCH, 
    ACK_TIMEOUT,
    TX_BUFFER_OVERFLOW, 
    PACKETS_RECEIVED,
    ACK_RECEIVED, 
    NACK_RECEIVED, 
    ACK_OUT_OF_RANGE, 
    DLC_OVER_CAPACITY,
    CRC_ERRORS,
    MSG_TIMEOUT,
    DELAYED_PACKET, 
    INJECTED_ERROR,
    VALID_TYPE,
    INVALID_TYPE,
    RESERVED_TYPE,  
    INHIBITED_MESSAGE,
    WDT_TIMEOUT,
    TOTAL_ERRORS
};

enum EVALUATION_TYPE : uint8_t{
    EQUAL,
    GREATER_THAN,
    LESS_THAN
};

// /////////////////////////////////////
//          Logging API's             //
// /////////////////////////////////////

// Self-test return code router
void process_transport_rx_return_error(uint8_t return_code);
void process_transport_tx_return_error(TX_RETURN_CODES return_code);
void process_protocol_return_error(uint8_t return_code);

// Self-test error logging
void st_clear_log();
void st_print_log();
void st_enable_logging();      
void st_disable_logging();    
void st_log_injected_error(); 
void st_log_delayed_packet();
void st_log_wdt_triggered(); 

// self-test error checks
bool st_check_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type, uint8_t expected_result);
bool st_compare_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type, ST_TEST_ENTRY expected_result);

// Runtime error logging
void rt_clear_error_log(); 
// fetch rt error list();

///////////////////
// metrics API's // 
///////////////////

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