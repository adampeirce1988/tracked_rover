#include <Arduino.h>
#include "log.h"
#include "debug.h"
#include "debug_config.h"
#include "global_config.h"

#define DEBUG_FILE DBG_LOGS

// -----------------------------
// Global metrics instance
// -----------------------------
selftest_metrics_t transport_test_log = {
    .diagnostics_active = true,
    .packets_sent = 0,
    .packets_received = 0,
    .ack_sent = 0,
    .ack_received = 0,
    .tx_latency_counter = 0,
    .rx_latency_counter = 0,
    .tx_max_latency = 0,
    .tx_min_latency = UINT16_MAX,
    .tx_average_latency = 0,
    .tx_total_latency = 0,
    .rx_max_latency = 0,
    .rx_min_latency = UINT16_MAX,
    .rx_average_latency = 0,
    .rx_total_latency = 0,
    .tx_retries = 0,
    .ack_mismatch = 0,
    .ack_timeout = 0,
    .tx_buffer_overflow = 0,
    .invalid_type = 0,
    .ack_out_of_range = 0,
    .dlc_exceeded_max = 0,
    .payload_overflow = 0,
    .crc_error = 0,
    .rx_timeouts = 0,
    .total_errors = 0
};

// -----------------------------
// Internal helpers
// -----------------------------
static void update_tx_average_latency(uint16_t value) {
    transport_test_log.tx_latency_counter++;
    transport_test_log.tx_total_latency += value;
    transport_test_log.tx_average_latency =
    transport_test_log.tx_total_latency / transport_test_log.tx_latency_counter;
}

static void update_rx_average_latency(uint16_t value) {
    transport_test_log.rx_latency_counter++;
    transport_test_log.rx_total_latency += value;
    transport_test_log.rx_average_latency =
        transport_test_log.rx_total_latency / transport_test_log.rx_latency_counter;
}

void transport_selftest_log_clear(){
    transport_test_log.packets_sent = 0;
    transport_test_log.packets_received = 0;
    transport_test_log.ack_sent = 0;
    transport_test_log.ack_received = 0;
    transport_test_log.tx_latency_counter = 0;
    transport_test_log.rx_latency_counter = 0;
    transport_test_log.tx_max_latency = 0;
    transport_test_log.tx_min_latency = UINT16_MAX;
    transport_test_log.tx_average_latency = 0;
    transport_test_log.tx_total_latency = 0;
    transport_test_log.rx_max_latency = 0;
    transport_test_log.rx_min_latency = UINT16_MAX;
    transport_test_log.rx_average_latency = 0;
    transport_test_log.rx_total_latency = 0;
    transport_test_log.tx_retries = 0;
    transport_test_log.ack_mismatch = 0;
    transport_test_log.ack_timeout = 0;
    transport_test_log.tx_buffer_overflow = 0;
    transport_test_log.invalid_type = 0;
    transport_test_log.ack_out_of_range = 0;
    transport_test_log.dlc_exceeded_max = 0;
    transport_test_log.payload_overflow = 0;
    transport_test_log.crc_error = 0;
    transport_test_log.rx_timeouts = 0;
    transport_test_log.total_errors = 0;
}



// -----------------------------
// Main logging function
// -----------------------------
void ST_LOG_EVENT(LOG_EVENT event, uint32_t value) {
    switch (event) {

        case EVENT_PACKET_SENT: transport_test_log.packets_sent++; break;
        case EVENT_PACKET_RECEIVED: transport_test_log.packets_received++; break;
        case EVENT_ACK_SENT: transport_test_log.ack_sent++; break;
        case EVENT_ACK_RECEIVED: transport_test_log.ack_received++; transport_test_log.packets_received++; break;
        case EVENT_TX_MAX_RETIRES: transport_test_log.tx_retries++; transport_test_log.total_errors++; break;
        case EVENT_ACK_MISMATCH: transport_test_log.ack_mismatch++; transport_test_log.total_errors++;break;
        case EVENT_ACK_TIMEOUT: transport_test_log.ack_timeout++; transport_test_log.total_errors++;break;
        case EVENT_TX_BUFF_OVERFLOW: transport_test_log.tx_buffer_overflow++; transport_test_log.total_errors++; break;
        case EVENT_INVALID_TYPE: transport_test_log.invalid_type++; transport_test_log.total_errors++; break;
        case EVENT_ACK_OUT_OF_RANGE: transport_test_log.ack_out_of_range++; transport_test_log.total_errors++; break;
        case EVENT_DLC_EXCEDED_MAX: transport_test_log.dlc_exceeded_max++; transport_test_log.total_errors++; break;
        case EVENT_PAYLOAD_OVERFLOW: transport_test_log.payload_overflow++; transport_test_log.total_errors++; break;
        case EVENT_CRC_ERROR: transport_test_log.crc_error++; transport_test_log.total_errors++; break;
        case EVENT_RX_TIMEOUT: transport_test_log.rx_timeouts++; transport_test_log.total_errors++; break;

        case EVENT_TX_LATANCY:
            if (value == 0) {
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "LOGS", "tx_latency updated with 0");
                break;
            }
            if (value > transport_test_log.tx_max_latency){
                transport_test_log.tx_max_latency = value;
            }
            if (value < transport_test_log.tx_min_latency){
                transport_test_log.tx_min_latency = value;
            }

            update_tx_average_latency(value);
            break;

        case EVENT_RX_LATANCY:
            if (value == 0) {
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "LOGS", "rx_latency updated with 0");
                break;
            }
            if (value > transport_test_log.rx_max_latency){
                transport_test_log.rx_max_latency = value;
            }
            if (value < transport_test_log.rx_min_latency){
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "LOGS", "rx_latency_min latancty: ", transport_test_log.rx_min_latency);    
                transport_test_log.rx_min_latency = value;
            }
            
            update_rx_average_latency(value);
        break;

        case EVENT_CLEAR_LOG:
            transport_test_log = (selftest_metrics_t){0};
            transport_test_log.tx_min_latency = UINT16_MAX;
            transport_test_log.rx_min_latency = UINT16_MAX;
            break;
    }
}

// -----------------------------
//  API  functions 
// -----------------------------

//activate loging 
void set_transport_selftest_loging_active() {
    transport_test_log.diagnostics_active = true;
}

// disable loging
void set_transport_selftest_loging_inactive() {
    transport_test_log.diagnostics_active = false;
}
