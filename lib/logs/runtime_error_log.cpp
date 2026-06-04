
#include <stdint.h>
#include "global.h"
#include "runtime_error_log.h"

struct ERROR{
    uint8_t count = 0;
    uint32_t first_occorance = 0;
    uint32_t last_occourance = 0;
    bool latch = false;
};


struct _RT_LOG{

    
    // transport TX errors
    uint8_t ack_mismatch = 0;
    uint8_t ack_not_received = 0;
    uint8_t ack_timeout = 0;
    uint8_t tx_buffer_overflow = 0;

    // transpoert RX errors
    uint8_t ack_out_of_range = 0;
    uint8_t dlc_exceeded_max = 0;
    uint8_t payload_overflow = 0;
    uint8_t crc_error = 0;
    uint8_t rx_timeouts = 0;

    // protocol errors
    uint8_t invalid_type = 0;
};

// struct declerations 
static _RT_LOG RT_LOG; 

void rt_log_event(RT_LOG_EVENT event){

    if(!sys::diagnostics_active){
    
        switch(event){
            case RT_LOG_EVENT::EVENT_ACK_NOT_RECEIVED:
                // system errors go here 
            break; 

            case RT_LOG_EVENT::EVENT_LOG_CLEAR:
                // clear log errors here
            break; 

            default:
                break; 

        }
    }
}


void rt_log_clear(){
    rt_log_event(RT_LOG_EVENT::EVENT_LOG_CLEAR);
}