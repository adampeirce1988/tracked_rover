#include <Arduino.h>
#include <stdint.h>
#include "debug.h"
#include "transport.h"
#include "self_test_internal.h"
#include "self_test.h"


#define DEBUG_FILE DBG_SELF_TEST


void check_self_test_resutls(TEST_RETURN_STATUS status_code){
    if(status_code == TEST_RETURN_STATUS::TEST_PASSED){
        self_test_state = TEST_OPERATION::TEST_RESULT_PASSED;
    }
    else if(status_code ==TEST_RETURN_STATUS::TEST_FAILED){
        self_test_state = TEST_OPERATION::TEST_RESULT_FAILED;
    }
}


uint8_t weighted_random_ack(){

    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_ACK_PERCENTAGE)  {return TRANSPORT_ACK_TYPE::ACK_REQUEST;}  // 20% chance of requiring ack 
    return TRANSPORT_ACK_TYPE::NORMAL_FRAME;               // 80% chance of normal frame 
}


uint16_t weighted_random_delay(uint16_t std_delay){
   
    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_DELAY_PERCENTAGE){
        uint16_t one_percent = std_delay / 100; 
        uint16_t delay_reduction = random((one_percent * 20), (one_percent * 35)); 
        //SELFTEST_LOG_EVENT(EVENT_DELAYED_PACKET);
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_INFO, "WRD", "Weighted randon delay value: ", delay_reduction, "us.");
        return delay_reduction; 
    }
    else{
        return 0; 
    }
}


// calculate inline crc using polynominoa 0x80
uint8_t self_test_inline_crc_calc(uint8_t CRC, uint8_t byte){
  CRC ^= byte;

  for (uint8_t i = 0; i < 8; i++) {
    if (CRC & 0x80){
     CRC = (CRC << 1) ^ 0x07;
    }
    else{
      CRC <<= 1;
    }
  }
  return CRC;
}


void test_request_dispatcher(TEST_ID requested_test_id){  // impliment to clean the seltest fsm change to request_test_case() 

    switch(requested_test_id){

        case TEST_ID::RETURN_TO_IDLE:    
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TEST", "no test case requested diagnostics exited without comopleting any test. requested_test_id: ", static_cast<uint8_t>(self_test::ctx.requested_test_id));        // rest the test cases here 
        self_test::ctx.active_test = TEST_OPERATION::IDLE; 
        break; 

        case TEST_ID::TRANSPORT_PACKET_NORMAL:     {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_GOOD_PACKET;}        break;
        case TEST_ID::TRANSPORT_PACKET_STRESS:     {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_STRESS_PACKET;}      break;
        case TEST_ID::TRANSPORT_TYPE_CHANGE:       {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_TYPE_CHANGE;}        break; 
        case TEST_ID::TRANSPORT_ACK_CHANGE:        {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_ACK_CHANGE;}         break; 
        case TEST_ID::TRANSPORT_ID_CHANGE:         {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_ID_CHANGE;}          break; 
        case TEST_ID::TRANSPORT_DLC_CHANG:         {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_DLC_CHANGE;}         break;
        case TEST_ID::TRANSPORT_DLC_OVERFLOW:      {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_DLC_OVERFLOW;}       break;
        case TEST_ID::TRANSPORT_CRC_BIT_FLIP:      {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_CRC_BIT_FLIP;}       break; 
        case TEST_ID::TRANSPORT_CRC_RAND_CHANGE:   {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_CRC_RANDOM_CHANGE;}  break; 
        case TEST_ID::TRANSPORT_DATA_BIT_FLIP:     {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_DATA_BIT_FLIP;}      break;
        case TEST_ID::TRANSPORT_DATA_BYTE_CHANGE:  {self_test::ctx.active_test = TEST_OPERATION::TRANSPORT_DATA_BYTE_CHANGE;}   break;  
        case TEST_ID::DIAGNOSTIC_WATCHDOG_TIMEOUT: {self_test::ctx.active_test = TEST_OPERATION::DIAGNOSTIC_WDT_TIMEOUT;}       break; 

        default: {DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "DEFAULT: requested test case is out of range.");}             break;         

    }

    // set diagnostic_active if a test is selsected
    if(self_test::ctx.active_test != TEST_OPERATION::IDLE){
        self_test::ctx.diagnostics_active = true; 
    }
}


void flush_self_test_struct(){
    // use this to flush the stuct.
}