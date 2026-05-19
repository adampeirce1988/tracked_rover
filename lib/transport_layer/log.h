#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include "debug.h"

// -----------------------------
// Event enum
// -----------------------------
enum LOG_EVENT { 
    EVENT_PACKET_SENT,
    EVENT_PACKET_RECEIVED, 
    EVENT_ACK_SENT,
    EVENT_DELAYED_PACKET,
    EVENT_ACK_RECEIVED, 
    EVENT_TX_MAX_RETIRES,
    EVENT_ACK_MISMATCH,
    EVENT_ACK_TIMEOUT,
    EVENT_TX_BUFF_OVERFLOW,
    EVENT_INVALID_TYPE,
    EVENT_ACK_OUT_OF_RANGE, 
    EVENT_DLC_EXCEDED_MAX,
    EVENT_PAYLOAD_OVERFLOW,
    EVENT_CRC_ERROR,
    EVENT_WDT_TRIGERED, 
    EVENT_RX_TIMEOUT,
    EVENT_TX_LATANCY,
    EVENT_RX_LATANCY,
    EVENT_CLEAR_LOG
};

// -----------------------------
// Self-test metrics structure
// -----------------------------
typedef struct selftest_metrics {

    bool diagnostics_active;

    // Counters
    uint8_t packets_sent;
    uint8_t packets_received;
    uint8_t ack_sent;
    uint8_t ack_received;

    // Latency tracking
    uint8_t tx_latency_counter;
    uint8_t rx_latency_counter;
    uint16_t tx_max_latency;
    uint16_t tx_min_latency;
    uint16_t tx_average_latency;
    uint32_t tx_total_latency;
    uint16_t rx_max_latency;
    uint16_t rx_min_latency;
    uint16_t rx_average_latency;
    uint32_t rx_total_latency;
    uint8_t delayed_packets;

    // TX failures
    uint8_t tx_retries;
    uint8_t ack_mismatch;
    uint8_t ack_timeout;
    uint8_t tx_buffer_overflow;

    // RX failures
    uint8_t invalid_type;
    uint8_t ack_out_of_range;
    uint8_t dlc_exceeded_max;
    uint8_t payload_overflow;
    uint8_t crc_error;
    uint8_t rx_timeouts;

    //total errors
    uint8_t total_errors; 

    //Watchow test active flags 
    bool diagnostics_wdt_test_result; 

} selftest_metrics_t;

// Global instance (defined in log.cpp)
extern selftest_metrics_t transport_test_log;

// -----------------------------
// Logging API
// -----------------------------
void ST_LOG_EVENT(LOG_EVENT event, uint32_t value = 0);
void transport_selftest_log_clear();
void set_transport_selftest_loging_active();
void set_transport_selftest_loging_inactive();

// -----------------------------
// Logging macros
// -----------------------------
#define SELFTEST_LOG_EVENT(event) do{ST_LOG_EVENT(event, 0);} while (0)

#define SELFTEST_LOG_EVENT_VAL(event, value) do{ST_LOG_EVENT(event, value);} while (0)


#define PRINT_TRANSPORT_SELFTEST_LOG() \
do { \
    DEBUG_PORT.print("packets sent:       "); DEBUG_PORT.println(transport_test_log.packets_sent); \
    DEBUG_PORT.print("packets received:   "); DEBUG_PORT.println(transport_test_log.packets_received); \
    DEBUG_PORT.print("ack sent:           "); DEBUG_PORT.println(transport_test_log.ack_sent); \
    DEBUG_PORT.print("ack received:       "); DEBUG_PORT.println(transport_test_log.ack_received); \
    DEBUG_PORT.print("tx max latency(us): "); DEBUG_PORT.println(transport_test_log.tx_max_latency); \
    DEBUG_PORT.print("tx min latency(us): "); DEBUG_PORT.println(transport_test_log.tx_min_latency); \
    DEBUG_PORT.print("tx avg latency(us): "); DEBUG_PORT.println(transport_test_log.tx_average_latency); \
    DEBUG_PORT.print("rx max latency(us): "); DEBUG_PORT.println(transport_test_log.rx_max_latency); \
    DEBUG_PORT.print("rx min latency(us): "); DEBUG_PORT.println(transport_test_log.rx_min_latency); \
    DEBUG_PORT.print("rx avg latency(us): "); DEBUG_PORT.println(transport_test_log.rx_average_latency); \
    DEBUG_PORT.print("delayed packets:    "); DEBUG_PORT.println(transport_test_log.delayed_packets); \
    DEBUG_PORT.print("ack mismatch:       "); DEBUG_PORT.println(transport_test_log.ack_mismatch); \
    DEBUG_PORT.print("ack timeout:        "); DEBUG_PORT.println(transport_test_log.ack_timeout); \
    DEBUG_PORT.print("tx buffer overflow: "); DEBUG_PORT.println(transport_test_log.tx_buffer_overflow); \
    DEBUG_PORT.print("invalid type:       "); DEBUG_PORT.println(transport_test_log.invalid_type); \
    DEBUG_PORT.print("ack out of range:   "); DEBUG_PORT.println(transport_test_log.ack_out_of_range); \
    DEBUG_PORT.print("dlc exceeded max:   "); DEBUG_PORT.println(transport_test_log.dlc_exceeded_max); \
    DEBUG_PORT.print("payload overflow:   "); DEBUG_PORT.println(transport_test_log.payload_overflow); \
    DEBUG_PORT.print("crc error:          "); DEBUG_PORT.println(transport_test_log.crc_error); \
    DEBUG_PORT.print("rx timeouts:        "); DEBUG_PORT.println(transport_test_log.rx_timeouts); \
    DEBUG_PORT.print("total error count   "); DEBUG_PORT.println(transport_test_log.total_errors); \
} while (0)

#endif
