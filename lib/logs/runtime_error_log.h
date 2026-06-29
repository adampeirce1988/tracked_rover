#ifndef RUNTIME_ERROR_LOG_H
#define RUNTIME_ERROR_LOG_H

#include <stdint.h>

//////////////////////////////////////////////////////
//                    structs                       //
//////////////////////////////////////////////////////

// error struct 
struct RUNTIME_ERROR{
    uint8_t count = 0;
    uint32_t timesramp_first = 0;
    uint32_t timestamp_last = 0;
    bool latch = false;
};

// error index
namespace ERROR_ID{ 
    enum ID : uint16_t{
        ACK_OUT_OF_RANGE,
        ACK_NOT_RECEIVED, // ack not received 
        ACK_WDT_TIMEOUT, // retry set active latch
        DLC_OVER_CAPACITY, 
        PAYLOAD_OVERFLOW, 
        ACK_MISMATCH,
        CRC_ERROR, 
        TX_BUFFER_OVERFLOW,
        MSG_TIMEOUT, 
        TYPE_INVALID,
        COUNT
    };
}


//////////////////////////////////////////////////////
//                extern declarations               //
//////////////////////////////////////////////////////

// array holding error structs
extern RUNTIME_ERROR rt_error_log_array[ERROR_ID::COUNT];


//////////////////////////////////////////////////////
//                forward declarations              //
//////////////////////////////////////////////////////

// forward declerations 
void rt_log_error(RUNTIME_ERROR &s, bool latch_active_error); 
void rt_clear_latch(RUNTIME_ERROR &s);

#endif