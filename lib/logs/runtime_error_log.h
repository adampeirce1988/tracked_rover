#ifndef RUNTIME_ERROR_LOG_H
#define RUNTIME_ERROR_LOG_H


enum class RT_LOG_EVENT{
    // TX Error event       
    EVENT_ACK_NOT_RECEIVED,    
    EVENT_ACK_MISMATCH,
    EVENT_ACK_WDT_TIEOUT,
    EVENT_TX_BUFFER_OVERFLOW,

    //RX error event
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
void rt_log_event(RT_LOG_EVENT event); 


#endif