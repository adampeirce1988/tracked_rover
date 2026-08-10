#include <Arduino.h>
#include "debug.h"
#include "transport.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST

/*=============================================================================*
 * Selftest functions
 *=============================================================================*/

uint8_t weighted_random_ack(){

    uint8_t r = random(0, 101); // Random value in range [0,100]

    if(r < RANDOM_ACK_PERCENTAGE)  {return TRANSPORT_ACK_TYPE::ACK_REQUEST;}  // 20% chance of requiring ack 
    return TRANSPORT_ACK_TYPE::NORMAL_FRAME;               // 80% chance of normal frame 
}


uint16_t weighted_random_delay(uint16_t std_delay){
   
    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_DELAY_PERCENTAGE){
        uint16_t one_percent = std_delay / 100; 
        uint16_t delay_reduction = random((one_percent * 20), (one_percent * 35)); 

        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_INFO, "WRD", "Weighted randon delay value: ", delay_reduction, "us.");

        return delay_reduction; 
    }
    
    return 0; 
}


const char* get_test_name(TEST_ID active_test){
    switch (active_test){
        case TEST_ID::NO_TEST_SELECTED:             {return "No test selected";}
        case TEST_ID::TRANSPORT_PACKET_NORMAL:      {return "Transport Packet Normal";}
        case TEST_ID::TRANSPORT_PACKET_STRESS:      {return "Transport Packet Stress";}
        case TEST_ID::TRANSPORT_TYPE_CHANGE:        {return "Transport Type Change";}
        case TEST_ID::TRANSPORT_ACK_CHANGE:         {return "Transport ACK Change";}
        case TEST_ID::TRANSPORT_ID_CHANGE:          {return "Transport ID Change";}
        case TEST_ID::TRANSPORT_DLC_CHANGE:         {return "Transport DLC Change";}
        case TEST_ID::TRANSPORT_DLC_OVERFLOW:       {return "Transport DLC Overflow";}
        case TEST_ID::TRANSPORT_CRC_BIT_FLIP:       {return "Transport CRC Bit Flip";}
        case TEST_ID::TRANSPORT_CRC_RANDOM_CHANGE:  {return "Transport CRC Random Change";}
        case TEST_ID::TRANSPORT_DATA_BIT_FLIP:      {return "Transport Data Bit Flip";}
        case TEST_ID::TRANSPORT_DATA_BYTE_CHANGE:   {return "Transport Data Byte Change";}
        case TEST_ID::DIAGNOSTIC_WATCHDOG_TIMEOUT:  {return "Diagnostic Watchdog Timeout";}
        default: {return "Unknown";}
    }
}

void manager_runtime_monitor(){
    // monitored during RUNNING state
    if(self_test::manager_ctx.manager_state != SELF_TEST_MANAGER::RUNNING){
        return; 
    }

    // Check the watchdog timer while the test is running.
    if(millis() - self_test::runtime_ctx.watchdog_timestamp > DIAGNOSTIC_WT_TIMEOUT_MS ){
        self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::TIMEOUT; 
    } 

    // Check for user abort flag during test running
    else if(self_test::runtime_ctx.user_aborted_test){
        self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::ABORTED;
    }
    
}

void clear_self_test_runtime_context(){
    self_test::runtime_ctx = {}; 
}

void abort_current_test(){
    self_test::runtime_ctx.user_aborted_test = true; 
}