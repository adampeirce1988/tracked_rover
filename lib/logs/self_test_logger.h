
#ifndef SELF_TEST_LOGGER_H 
#define SELF_TEST_LOGGER_H

#include <stdint.h>

// TODO EXPOSE THESE ENUM FOR USAGE IN GLOBAL SPACE

namespace LOG_TYPE{
    constexpr bool ERROR = true; 
    constexpr bool METRIC = false; 
}

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

    //Test error logging 
    EVENT_PACKECT_DELAYED,
    EVENT_ERROR_INJECTED,

    //Reset logs
    EVENT_LOG_CLEAR // clear all log files.
};

// enum class ST_TEST_ENTRY {
//     ST_LOG_PACKETS_SENT,
//     ST_LOG_ACKS_TRANSMITTED,
//     ST_LOG_RETRY_ATTEMPT,
//     ST_LOG_ACK_NOT_RECEIVED, 
//     ST_LOG_ACK_MISMATCH, 
//     ST_LOG_ACK_TIMEOUT,
//     ST_LOG_TX_BUFFER_OVERFLOW, 
//     ST_LOG_PACKETES_RECEIVED,
//     ST_LOG_ACK_RECEIVED, 
//     ST_LOG_NACK_RECEIVED, 
//     ST_LOG_INVALID_TPYE, 
//     ST_LOG_ACK_OUT_OF_RANGE, 
//     ST_LOG_DLC_OVER_CAPACITY,
//     ST_LOG_CRC_ERRORS,
//     ST_LOG_MSG_TIMEOUT,
//     ST_LOG_TOTAL_ERRORS
// };

// enum class EVALUATION_TYPE{
//     EQUAL,
//     GREATER_THAN,
//     LESS_THAN
// };

// forward declerations
void st_log_event (ST_LOG_EVENT event, bool log_type);

#endif