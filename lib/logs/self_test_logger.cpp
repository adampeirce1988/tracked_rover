

#include <stdint.h>
#include "self_test_logger.h"
#include "logger.h"
#include "metrics.h"
#include "transport.h"
#include "global.h"
#include "debug.h"

#define DEBUG_FILE DBG_LOGS

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
void st_log_event(ST_LOG_EVENT event, bool log_type){ 
    if(sys::diagnostics_active){
        switch(event){
            // TX Logging 
            case ST_LOG_EVENT::EVENT_PACKET_SENT:         { ST_LOG.packets_sent++; }        break; 
            case ST_LOG_EVENT::EVENT_ACK_SENT:            { ST_LOG.ack_transmitted++;}      break; // error here 
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

            // test logging 
            case ST_LOG_EVENT::EVENT_PACKECT_DELAYED:     {ST_LOG.delayed_packets ++;}      break; 
            case ST_LOG_EVENT::EVENT_ERROR_INJECTED:      {ST_LOG.injected_error ++;}       break; 
            
            // clear error log
            case ST_LOG_EVENT::EVENT_LOG_CLEAR:           { ST_LOG = {};}                   break;
        }   

        // increment total error is fault flag is true
        if(log_type == LOG_TYPE::ERROR){
            ST_LOG.total_errors++; 

        }
    }
}


uint8_t get_test_value(ST_TEST_ENTRY entry){
    switch(entry){
        // set test value here before test occours
        case ST_TEST_ENTRY::ST_LOG_PACKETS_SENT:       {return ST_LOG.packets_sent;}       break; 
        case ST_TEST_ENTRY::ST_LOG_ACKS_TRANSMITTED:   {return ST_LOG.ack_transmitted;}    break; 
        case ST_TEST_ENTRY::ST_LOG_RETRY_ATTEMPT:      {return ST_LOG.retry_attempt;}      break; 
        case ST_TEST_ENTRY::ST_LOG_ACK_NOT_RECEIVED:   {return ST_LOG.ack_not_received;}   break; 
        case ST_TEST_ENTRY::ST_LOG_ACK_MISMATCH:       {return ST_LOG.ack_mismatch;}       break; 
        case ST_TEST_ENTRY::ST_LOG_ACK_TIMEOUT:        {return ST_LOG.ack_timeout;}        break;
        case ST_TEST_ENTRY::ST_LOG_TX_BUFFER_OVERFLOW: {return ST_LOG.tx_buffer_overflow;} break; 

        case ST_TEST_ENTRY::ST_LOG_DELAYED_PACKET:     {return ST_LOG.delayed_packets;}    break; 
        case ST_TEST_ENTRY::ST_LOG_INJECTED_ERROR:     {return ST_LOG.injected_error;}     break; 

        case ST_TEST_ENTRY::ST_LOG_PACKETES_RECEIVED:  {return ST_LOG.packets_received;}   break; 
        case ST_TEST_ENTRY::ST_LOG_ACK_RECEIVED:       {return ST_LOG.ack_received;}       break;
        case ST_TEST_ENTRY::ST_LOG_NACK_RECEIVED:      {return ST_LOG.nack_received;}      break; 
        case ST_TEST_ENTRY::ST_LOG_INVALID_TPYE:       {return ST_LOG.invalid_type;}       break;
        case ST_TEST_ENTRY::ST_LOG_ACK_OUT_OF_RANGE:   {return ST_LOG.ack_out_of_range;}   break;
        case ST_TEST_ENTRY::ST_LOG_DLC_OVER_CAPACITY:  {return ST_LOG.dlc_exceeded_max;}   break; 
        case ST_TEST_ENTRY::ST_LOG_CRC_ERRORS:         {return ST_LOG.crc_error;}          break; 
        case ST_TEST_ENTRY::ST_LOG_MSG_TIMEOUT:        {return ST_LOG.rx_timeouts;}        break; 
        case ST_TEST_ENTRY::ST_LOG_TOTAL_ERRORS:       {return ST_LOG.total_errors;}       break; 



        default: 
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "LOG", "ST_TEST_ENTRY invalid entry");
            break; 
    }
    return UINT8_MAX;
}


bool st_check_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type, uint8_t expected_result){

    uint8_t test_value = get_test_value(entry); 
    
    if(evaluation_type == EVALUATION_TYPE::EQUAL){
        if(test_value == expected_result) {
            return true;
        }
    }
    else if(evaluation_type == EVALUATION_TYPE::GREATER_THAN){
        if(test_value > expected_result){
            return true; 
        }
    }
    else if(evaluation_type == EVALUATION_TYPE::LESS_THAN){
        if(test_value < expected_result){
            return true; 
        }
    }
    else {
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "LOG", "evaluation type invalid");
    }

    return false; 
}

bool st_compare_test_result(ST_TEST_ENTRY entry, EVALUATION_TYPE evaluation_type, ST_TEST_ENTRY expected_result){

    uint8_t test_value_1 = get_test_value(entry); 
    uint8_t test_value_2 = get_test_value(expected_result);
    
    if(evaluation_type == EVALUATION_TYPE::EQUAL){
        if(test_value_1 == test_value_2) {
            return true;
        }
    }
    else if(evaluation_type == EVALUATION_TYPE::GREATER_THAN){
        if(test_value_1 > test_value_2){
            return true; 
        }
    }
    else if(evaluation_type == EVALUATION_TYPE::LESS_THAN){
        if(test_value_1 < test_value_2){
            return true; 
        }
    }
    else {
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "LOG", "evaluation type invalid");
    }

    return false; 
}
    
void st_clear_log(){
    st_log_event(ST_LOG_EVENT::EVENT_LOG_CLEAR, LOG_TYPE::METRIC);
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
        PRINT_LOG_ENTRY("tx max latancy(us):....", latency_get_max(tx_latency_buffer)); 
        PRINT_LOG_ENTRY("tx min latancy(us):....", latency_get_min(tx_latency_buffer));
        PRINT_LOG_ENTRY("tx avragelatancy(us):..", latency_get_average(tx_latency_buffer ));
        PRINT_LOG_ENTRY("tx jitter(us):.........", latency_get_jitter(tx_latency_buffer ));
        PRINT_LOG_ENTRY("rx max latancy(us):....", latency_get_max(rx_latency_buffer));
        PRINT_LOG_ENTRY("rx min latancy(us):....", latency_get_min(rx_latency_buffer));
        PRINT_LOG_ENTRY("rx avragelatancy(us):..", latency_get_average(rx_latency_buffer));
        PRINT_LOG_ENTRY("rx jitter(us):.........", latency_get_jitter(rx_latency_buffer));       
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
        PRINT_LOG_ENTRY("reserved type:.........",ST_LOG.reserved_type);
        // total errors
        PRINT_LOG_ENTRY("total errors:..........",ST_LOG.total_errors);
        PRINT_LOG_FOOTER(); 
}


// external API calls - look to intergrate into current calls
void st_log_injected_error(){
    st_log_event(ST_LOG_EVENT::EVENT_ERROR_INJECTED, LOG_TYPE::METRIC);
}

void st_log_delayed_packet(){
    st_log_event(ST_LOG_EVENT::EVENT_PACKECT_DELAYED, LOG_TYPE::METRIC);
}