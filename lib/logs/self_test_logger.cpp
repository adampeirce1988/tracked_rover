

#include <stdint.h>
#include "self_test_logger.h"
#include "logger.h"
#include "transport.h" 
#include "global.h"
#include "debug.h"

// strucs 
struct _ST_LOG{
    // self test metics
    uint8_t packets_sent = 0; 
    uint8_t packets_received = 0; 
    uint8_t ack_received = 0;
    uint8_t nack_received = 0; 
    uint8_t ack_transmitted = 0; 
    uint8_t retry_attempt = 0; 
    uint8_t ack_valid = 0;    
    uint8_t valid_type = 0; 
    
    // selftet errors
    uint8_t delayed_packets = 0; // no return code implimented for this 
    uint8_t injected_error = 0; // no return code avaliavle for this.

    // TX failures
    uint8_t ack_mismatch = 0;
    uint8_t ack_not_received = 0;
    uint8_t ack_timeout = 0;
    uint8_t tx_buffer_overflow = 0;

    // RX failures
    uint8_t ack_out_of_range = 0;
    uint8_t dlc_exceeded_max = 0;
    uint8_t payload_overflow = 0;
    uint8_t crc_error = 0;
    uint8_t rx_timeouts = 0;
    
    // Protocaol errors 
    uint8_t invalid_type = 0; 
    uint8_t inhibited_message = 0; 
    uint8_t reserved_type = 0;

    // total erro count
    uint8_t total_errors = 0;
};

// struct declerations 
static _ST_LOG ST_LOG; 


// functions 
void st_log_event(ST_LOG_EVENT event, bool log_type = LOG_TYPE::METRIC){ 
    if(sys::diagnostics_active){
        switch(event){
            
            // TX Logging 
            case ST_LOG_EVENT::EVENT_PACKET_SENT:         { ST_LOG.packets_sent++; }        break; 
            case ST_LOG_EVENT::EVENT_ACK_SENT:            { ST_LOG.ack_transmitted++;}      break; 
            case ST_LOG_EVENT::EVENT_RETRY_SENT:          { ST_LOG.retry_attempt++;}        break;
            case ST_LOG_EVENT::EVENT_ACK_NOT_RECEIVED:    { ST_LOG.ack_not_received++; }    break;
            case ST_LOG_EVENT::EVENT_ACK_MISMATCH:        { ST_LOG.ack_mismatch++; }        break;
            case ST_LOG_EVENT::EVENT_ACK_WDT_TIEOUT:      { ST_LOG.ack_timeout++;}          break;
            case ST_LOG_EVENT::EVENT_TX_BUFFER_OVERFLOW:  { ST_LOG.tx_buffer_overflow++; }  break; 

            // RX Logging 
            case ST_LOG_EVENT::EVENT_PACKET_RECEIVED:     { ST_LOG.packets_received++; }    break; 
            case ST_LOG_EVENT::EVENT_ACK_RECEIVED:        { ST_LOG.ack_received++; }        break; 
            case ST_LOG_EVENT::EVENT_NACK_REVEIVED:       { ST_LOG.nack_received++; }       break; 
            case ST_LOG_EVENT::EVENT_INVALID_TYPE:        { ST_LOG.invalid_type++; }        break; 
            case ST_LOG_EVENT::EVENT_ACK_OUT_OF_RANGE:    { ST_LOG.ack_out_of_range++; }    break; 
            case ST_LOG_EVENT::EVENT_DLC_OVER_CAPACITY:   { ST_LOG.dlc_exceeded_max++;}     break;
            case ST_LOG_EVENT::EVENT_PAYLOAD_OVERFLOW:    { ST_LOG.payload_overflow++; }    break; 
            case ST_LOG_EVENT::EVENT_CRC_ERROR:           { ST_LOG.crc_error++;}            break;
            case ST_LOG_EVENT::EVENT_MSG_TIMEOUT_ERROR:   { ST_LOG.rx_timeouts++;}          break;

            // clear error log
            case ST_LOG_EVENT::EVENT_LOG_CLEAR:
                // clear metrics
                ST_LOG.packets_sent = 0; 
                ST_LOG.packets_received = 0; 
                ST_LOG.ack_received = 0;
                ST_LOG.nack_received = 0; 
                ST_LOG.ack_transmitted = 0; 
                ST_LOG.retry_attempt = 0; 
                ST_LOG.ack_valid = 0;
                ST_LOG.valid_type = 0;   

                // test metrics
                ST_LOG.injected_error = 0;
                ST_LOG.delayed_packets = 0;

                //clear TX errors 
                ST_LOG.ack_mismatch = 0;
                ST_LOG.ack_not_received = 0;
                ST_LOG.ack_timeout = 0;
                ST_LOG.tx_buffer_overflow = 0;

                // RX failures
                ST_LOG.invalid_type = 0;
                ST_LOG.ack_out_of_range = 0;
                ST_LOG.dlc_exceeded_max = 0;
                ST_LOG.payload_overflow = 0;
                ST_LOG.crc_error = 0;
                ST_LOG.rx_timeouts = 0;


                // protocol errors
                ST_LOG.invalid_type = 0; 
                ST_LOG.inhibited_message = 0; 
                ST_LOG.reserved_type = 0;
                
                // total error count
                ST_LOG.total_errors = 0;

            break;
        }   

        // increment total error is fault flag is true
        if(log_type == LOG_TYPE::ERROR){
            ST_LOG.total_errors++; 

        }
    }
    return; 
}


void st_log_clear(){
    st_log_event(ST_LOG_EVENT::EVENT_LOG_CLEAR);
}

void st_print_log(){
    // Print header
        PRINT_ST_LOG_HEARDER(); 
    // Test metrics
        PRINT_LOG_ENTRY("packets sent:..........",ST_LOG.packets_sent);
        PRINT_LOG_ENTRY("packets received:......",ST_LOG.packets_received);
        PRINT_LOG_ENTRY("ACKs transmitted:......",ST_LOG.ack_transmitted); 
        PRINT_LOG_ENTRY("ACKs received:.........",ST_LOG.ack_received);
        PRINT_LOG_ENTRY("ACKs validated:........",ST_LOG.ack_valid);
        PRINT_LOG_ENTRY("NACKs received:........",ST_LOG.nack_received);
        PRINT_LOG_ENTRY("retries attempted:.....",ST_LOG.retry_attempt);
    // System metrics
        PRINT_LOG_SEPERATOR();
        PRINT_LOG_ENTRY("tx max latancy:........", tx_latency_get_max());
        PRINT_LOG_ENTRY("tx min latancy:........", tx_latency_get_min());
        PRINT_LOG_ENTRY("tx avragelatancy:......", tx_latency_get_average());
        PRINT_LOG_ENTRY("tx jitter:.............", tx_latency_get_jitter());
        PRINT_LOG_ENTRY("rx max latancy:........", rx_latency_get_max());
        PRINT_LOG_ENTRY("rx min latancy:........", rx_latency_get_min());
        PRINT_LOG_ENTRY("rx avragelatancy:......", rx_latency_get_average());
        PRINT_LOG_ENTRY("rx jitter:.............", rx_latency_get_jitter());      
    // Errors metrics
        PRINT_LOG_SEPERATOR();
        PRINT_LOG_ENTRY("injected errors........",ST_LOG.injected_error);
        PRINT_LOG_ENTRY("packets delayed:.......",ST_LOG.delayed_packets);
    // Error types
        // TX errors
        PRINT_LOG_SEPERATOR();
        PRINT_LOG_ENTRY("ACK mismatched.........",ST_LOG.ack_mismatch);
        PRINT_LOG_ENTRY("ACK not received.......",ST_LOG.ack_not_received);
        PRINT_LOG_ENTRY("ACK timeout............",ST_LOG.ack_timeout);
        PRINT_LOG_ENTRY("tx buffer overflow.....",ST_LOG.tx_buffer_overflow);     
        // RX errors
        PRINT_LOG_ENTRY("invalid type...........",ST_LOG.invalid_type);
        PRINT_LOG_ENTRY("ACK out of range.......",ST_LOG.ack_out_of_range);
        PRINT_LOG_ENTRY("DLC exceded max........",ST_LOG.dlc_exceeded_max);
        PRINT_LOG_ENTRY("payload overflow.......",ST_LOG.payload_overflow);
        PRINT_LOG_ENTRY("CRC error..............",ST_LOG.crc_error);
        PRINT_LOG_ENTRY("RX timeout.............",ST_LOG.rx_timeouts);
        // Protocol errors
        PRINT_LOG_ENTRY("invalid type:..........",ST_LOG.invalid_type);
        PRINT_LOG_ENTRY("msg inhibited..........",ST_LOG.inhibited_message);
        PRINT_LOG_ENTRY("reserved type:..........",ST_LOG.reserved_type);
        // total errors
        PRINT_LOG_ENTRY("total errors:..........",ST_LOG.total_errors);
        PRINT_LOG_FOOTER(); 
}