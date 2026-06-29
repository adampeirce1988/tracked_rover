
#ifndef SELF_TEST_LOGGER_H 
#define SELF_TEST_LOGGER_H

#include <stdint.h>


enum class LOG_TYPE : bool
{
    ERROR = true,
    METRIC = false
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
    EVENT_ACK_OUT_OF_RANGE,
    EVENT_DLC_OVER_CAPACITY,
    EVENT_PAYLOAD_OVERFLOW,
    EVENT_CRC_ERROR,
    EVENT_MSG_TIMEOUT_ERROR,

    //protocol logging
    EVENT_VALID_TYPE,
    EVENT_INVALID_TYPE,
    EVENT_RESERVED_TYPE,
    EVENT_INHIBITED_MESSAGE,

    //Test error logging 
    EVENT_PACKECT_DELAYED,
    EVENT_ERROR_INJECTED,

    //Test wdt error 
    DIAG_WDT_TRIGERED,

    //Reset logs
    EVENT_LOG_CLEAR // clear all log files.
};


// forward declerations
void st_log_event (ST_LOG_EVENT event, LOG_TYPE log_type);

#endif