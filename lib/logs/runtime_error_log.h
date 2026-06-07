#ifndef RUNTIME_ERROR_LOG_H
#define RUNTIME_ERROR_LOG_H

#include <stdint.h>

struct ERROR_STRUCT{
    uint8_t count = 0;
    uint32_t first_occurrence = 0;
    uint32_t last_occurrence = 0;
    bool latch = false;
};

enum ERROR_ID{
    ERROR_ID_ACK_OUT_OF_RANGE,
    ERROR_ID_ACK_NOT_RECEIVED, // ack not received 
    ERROR_ID_ACK_WDT_TIMEOUT, // retry set active latch
    ERROR_ID_DLC_OVER_CAPACITY, 
    ERROR_ID_PAYLOAD_OVERFLOW, 
    ERROR_ID_ACK_MISMATCH,
    ERROR_ID_CRC_ERROR, 
    ERROR_ID_TX_BUFFER_OVERFLOW,
    ERROR_ID_MSG_TIMEOUT, 
    ERROR_ID_COUNT
};

extern ERROR_STRUCT rt_error_log_array[ERROR_ID_COUNT];

// forward declerations 
void rt_log_error(ERROR_STRUCT &s, bool latch_active_error); 
void rt_clear_latch(ERROR_STRUCT &s);
void rt_erase_error_codes();

#endif