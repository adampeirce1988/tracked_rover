
#ifndef SELF_TEST_LOGGER_H 
#define SELF_TEST_LOGGER_H

#include <stdint.h>

namespace LOG_TYPE{
constexpr bool ERROR = true; 
constexpr bool METRIC = false; 
};

enum class ST_LOG_EVENT{
    // TX EVENT
    EVENT_PACKET_SENT,     
    EVENT_ACK_SENT,         
    EVENT_RETRY_SENT,        
    EVENT_ACK_NOT_RECEIVED,    
    EVENT_ACK_MISMATCH,
    EVENT_ACK_WDT_TIEOUT,
    EVENT_TX_BUFFER_OVERFLOW,

    //RX EVENTs 
    EVENT_PACKET_RECEIVED, 
    EVENT_ACK_RECEIVED,
    EVENT_NACK_REVEIVED, 
    EVENT_INVALID_TYPE,
    EVENT_ACK_OUT_OF_RANGE,
    EVENT_DLC_OVER_CAPACITY,
    EVENT_PAYLOAD_OVERFLOW,
    EVENT_CRC_ERROR,
    EVENT_MSG_TIMEOUT_ERROR,

    //Reset logs
    EVENT_LOG_CLEAR // clear all log files.
};


// forward declerations
void st_log_event(ST_LOG_EVENT event, bool log_type);


#endif