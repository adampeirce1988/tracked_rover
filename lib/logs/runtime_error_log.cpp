
#include <stdint.h>
#include <Arduino.h>
#include "global.h"
#include "runtime_error_log.h"


RUNTIME_ERROR rt_error_log_array[ERROR_ID::COUNT]; 


// internal forward declerations
static void rt_log_entry_clear(RUNTIME_ERROR &s); 

   
void rt_log_error(RUNTIME_ERROR &s, bool latch_error = false){
    if(sys::diagnostics_active || s.latch){ // if diagnostics test active do not log or the fault is latched
        return; 
    }

    const uint32_t timestamp = millis();
    s.timestamp_last = timestamp; // change to numreicl time and date stamp

    if(s.count == 0){
        s.timesramp_first = timestamp; // change to numreicl time and date stamp
    }

    // Saturating handler. Once 255 is reached it remains at 255.
    if(s.count < UINT8_MAX){
        s.count ++;  
    }

    // latch the error untill a good packet is received or rest occours. ** TODO: ADD CLEAR ALL LATCHES ON BOOT **
    if(latch_error){ 
        s.latch = true; 
    }
}

void rt_log_entry_clear(RUNTIME_ERROR &s){
    if(s.count > 0){
        s.count = 0; 
        s.timesramp_first = 0;
        s.timestamp_last = 0; 
        s.latch = false;
    }
}

void rt_clear_latch(RUNTIME_ERROR &s){
    s.latch = false; 
}

void rt_clear_error_log(){
    for(uint8_t i = 0; i < ERROR_ID::COUNT; i ++){
        rt_log_entry_clear(rt_error_log_array[i]); 
    }
}
