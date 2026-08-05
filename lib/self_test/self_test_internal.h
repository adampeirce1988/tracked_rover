#ifndef SELF_TEST_INTERNAL_H
#define SELF_TEST_INTERNAL_H

#include <stdint.h>
#include "transport.h"
#include "self_test_types.h"

// ===============globl self test config ================
constexpr uint8_t PROGRESS_BAR_COUNT =              25;
// =======================================================

//************* global test congifg *************
constexpr uint32_t TEST_END_COUNTDOWN_TIMER_US =     500000;   // 0.5 seconds
constexpr uint32_t TEST_START_COUNTDOWN_TIMER_US =   500000;   // 0.5 seconds 
constexpr uint16_t DEFAULT_PACKET_DELAY_US =         15000;
//***********************************************

//********** random percentage congifg **********
constexpr uint8_t RANDOM_ACK_PERCENTAGE =            20; //% chance 
constexpr uint8_t RANDOM_DELAY_PERCENTAGE =          20; //% chance
//***********************************************

//***************** packet timing test ***************** 
constexpr uint8_t STANDARD_TEST_PACKET_COUNT =       200;
constexpr uint8_t STRESS_TEST_PACKET_COUNT =         200;
constexpr uint16_t STANDARD_PACKET_DELAY_US =        DEFAULT_PACKET_DELAY_US; 
constexpr uint16_t STRESS_TEST_PACKET_DELAY_US =     5000;
constexpr bool ENABLE_RANDOM_PACKET_TIMING_DELAY =   true;
constexpr bool DISABLE_RANDOM_PACKET_TIMING_DELAY =  false;
//****************************************************** 

//**************** injection test config ***************** 
constexpr uint8_t INJECTION_TEST_PACKET_COUNT =            100;
constexpr uint16_t INJECTION_TEST_PACKET_DELAY_US =        20000;
constexpr uint8_t INJECTION_TEST_ERROR_COUNT =             INJECTION_TEST_PACKET_COUNT / 10; // 10% errors 
constexpr uint8_t CRC_RANDOM_REPLACEMENT_VALUE =           0xFE;  // change this to random and handle duplics/ matching 
//************************************************

//**************** Watchdog timer test config ***************** 
constexpr uint32_t WDT_EXTENSION = 250; 
//************************************************************* 

//********* internal function prototypes ************

//moved to internal from public
TEST_RETURN_STATUS execute_self_test();                            // advances selftest state macheine. stest state machine needs to be called every loop during a test
void clear_self_test_runtime_context();                        // flush the struct for self_test::ctx
const char* get_test_name(TEST_ID active_test);                // returns a string containing the test name
void test_request_dispatcher(TEST_ID requested_test_id);       // to be implimented still in selftest internal 
uint8_t weighted_random_ack();                                 // returns either NORMAL_FRAME or 20% chance of ACK_FRAME  
uint16_t weighted_random_delay(uint16_t std_delay);            // Creates random delay between packets for packet testing
void manager_runtime_monitor();
// Core test functions 
TEST_RETURN_STATUS self_test_transport_random_packet(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active);
TEST_RETURN_STATUS self_test_error_injection(uint8_t no_of_packets, uint8_t error_count, TX_FAULT_MODE fault_type, uint8_t fault_value = 0);
TEST_RETURN_STATUS self_test_diagnostics_wdt(); 

//****************************************************


//******************** variables *********************

struct SelfTestManagerContext {
  
    // test manager fsm variables 
    SELF_TEST_MANAGER_RETURN_CODE manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::ERROR;  // default to ERROR
    SELF_TEST_MANAGER manager_state = SELF_TEST_MANAGER::IDLE;

    // Progress reporting
    TEST_RETURN_STATUS current_test_status_code = TEST_RETURN_STATUS::IDLE;
};

struct SelfTestRuntimeContext{

    // Timing
    uint32_t next_transmission_time = 0;
    uint32_t test_start_countdown_timer = 0; 
    uint32_t test_end_countdown_timer = 0;
    uint32_t watchdog_timestamp = 0; 
    
    // Progress reporting
    uint8_t current_test_packet = 0;
    uint8_t progress_bar_position = 1; // impliment in all tests
    uint8_t progress_bar_one_percent = 0; //impliment in all tests

    // test variables
    uint8_t next_injection_position = 0;  // impliment in tests 

    // control flags 
    bool diagnostics_active = false;
    bool test_start_countdown_active = false; 
    bool test_end_countdown_active = false;
    bool watchdog_timer_test_active = false;
    bool watchdog_event_verified = false; 
    bool user_aborted_test = false; 

    // Test selection  
    TEST_ID requested_test_id = TEST_ID::NO_TEST_SELECTED;  
    TEST_ID current_active_test_id = TEST_ID::NO_TEST_SELECTED; 

    // Test result 
    TEST_RETURN_STATUS current_test_status_code = TEST_RETURN_STATUS::IDLE;
};

namespace self_test {
  extern SelfTestManagerContext manager_ctx;
  extern SelfTestRuntimeContext runtime_ctx;
}
//****************************************************

#endif 