#include <Arduino.h>
#include <stdint.h>
#include "log.h"
#include "debug.h"
#include "transport.h"
#include "self_test_internal.h"
#include "self_test.h"


#define DEBUG_FILE DBG_SELF_TEST

void check_self_test_resutls(uint8_t status_code){
    if(status_code == SELFTEST_PASSED){
        self_test_state = TEST_OPERATION::TEST_RESULT_PASSED;
    }
    else if(status_code == SELFTEST_FAILED){
        self_test_state = TEST_OPERATION::TEST_RESULT_FAILED;
    }
}

uint8_t weighted_random_ack(){

    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_ACK_PERCENTAGE)  {return ACK_REQUEST;}  // 20% chance of requiring ack 
    return NORMAL_FRAME;               // 80% chance of normal frame 
}

uint16_t weighted_random_delay(uint16_t std_delay){
   
    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_DELAY_PERCENTAGE){
        uint16_t one_percent = std_delay / 100; 
        uint16_t delay_reduction = random((one_percent * 20), (one_percent * 35)); 
        SELFTEST_LOG_EVENT(EVENT_DELAYED_PACKET);
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_INFO, "WRD", "Weighted randon delay value: ", delay_reduction, "us.");
        return delay_reduction; 
    }
    else{
        return 0; 
    }
}

