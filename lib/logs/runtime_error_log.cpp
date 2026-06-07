
#include <stdint.h>
#include <Arduino.h>
#include "global.h"
#include "runtime_error_log.h"

// internal forward declerations
static void rt_log_entry_clear(ERROR_STRUCT &s); 


//ERROR_STRUCT RT_ERROR_LOG[ERROR_ID_COUNT]; 
ERROR_STRUCT rt_error_log_array[ERROR_ID_COUNT]; 
   
void rt_log_error(ERROR_STRUCT &s, bool latch_error = false){
    if(sys::diagnostics_active || s.latch){ // if diagnostics test active do not log or the fault is latched
        return; 
    }

    const uint32_t timestamp = millis();
    s.last_occurrence = timestamp; // change to numreicl time and date stamp

    if(s.count == 0){
        s.first_occurrence = timestamp; // change to numreicl time and date stamp
    }

    if(s.count < UINT8_MAX){
        s.count ++;  
    }

    if(latch_error){ // latch the error untill a good packet is received. 
        s.latch = true; 
    }
}

void rt_log_entry_clear(ERROR_STRUCT &s){
    if(s.count > 0){
        s.count = 0; 
        s.first_occurrence = 0;
        s.last_occurrence = 0; 
        s.latch = false;
    }
}

void rt_clear_latch(ERROR_STRUCT &s){
    s.latch = false; 
}

void rt_erase_error_codes(){
    for(uint8_t i = 0; i < ERROR_ID_COUNT; i ++){
        rt_log_entry_clear(rt_error_log_array[i]); 
    }
}
