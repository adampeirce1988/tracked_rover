#ifndef SELF_TEST_INTERNAL_H
#define SELF_TEST_INTERNAL_H

#include <stdint.h>
#include "transport.h"
#include "self_test.h"

//************* global test congifg *************
constexpr uint32_t TEST_END_COUNTDOWN_TIMER_US =     500000;   // 0.5 seconds
constexpr uint16_t DEFAULT_PACKET_DELAY_US =         15000;
//***********************************************

//********** random percentage congifg **********
constexpr uint8_t RANDOM_ACK_PERCENTAGE =            20; //% chance 
constexpr uint8_t RANDOM_DELAY_PERCENTAGE =          20; //% chance
//***********************************************

//***************** packet timing test ***************** 
constexpr uint8_t TEST_1_PACKET_COUNT =              100;
constexpr uint16_t STANDARD_PACKET_DEALY_US =        DEFAULT_PACKET_DELAY_US; 
constexpr uint16_t STRESS_TEST_PACKET_DELAY_US =     5000;
constexpr bool ENABLE_RANDOM_PACKET_TIMING_DELAY =   true;
constexpr bool DISABLE_RANDOM_PACKET_TIMING_DELAY =  false;
//****************************************************** 

//**************** injection test config ***************** 
constexpr uint8_t INJECTION_PACKET_COUNT =            100;
constexpr uint16_t INJECTION_PACKET_DELAY_US =        20000;
constexpr uint8_t INJECTION_ERROR_COUNT =             INJECTION_PACKET_COUNT / 10; // 10% errors 
constexpr uint8_t TEST_9_ERROR_VALUE =                0xFE;  // change this to random and handle duplics/ matching 
//************************************************


//********* internal function prototypes ************
void test_request_dispatcher(TEST_ID requested_test_id); // to be implimented still in selftest internal 
void check_self_test_resutls(TEST_RETURN_STATUS status_code);
uint8_t weighted_random_ack();
uint16_t weighted_random_delay(uint16_t std_delay); 
uint8_t self_test_inline_crc_calc(uint8_t CRC, uint8_t byte);
TEST_RETURN_STATUS self_test_transport_random_packet(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active);
TEST_RETURN_STATUS self_test_error_injection(uint8_t no_of_packets, uint8_t error_count, TX_SET_FAULT_MODE fault_type, uint8_t fault_value = 0);
TEST_RETURN_STATUS self_test_diagnostics_wdt(); 

//****************************************************


//***************** test enums class ***************** 

// enum class TEST_OPERATION{ 
//     IDLE, 
//     TEST_RESULT_PASSED, 
//     TEST_RESULT_FAILED, 
//     END_SELF_TEST, 
//     TRANSPORT_SELF_TEST_1,    // good packet test 
//     TRANSPORT_SELF_TEST_2,    // good packet stress test
//     TRANSPORT_SELF_TEST_3,    // type change **NOT USED**
//     TRANSPORT_SELF_TEST_4,    // ack change  **NOT USED** 
//     TRANSPORT_SELF_TEST_5,    // id change   **NOT USED**
//     TRANSPORT_SELF_TEST_6,    // DLC change **NOT USED**
//     TRANSPORT_SELF_TEST_7,    // DLC over max capaicity. **NEXT TO IMP**
//     TRANSPORT_SELF_TEST_8,    // CRC crc bit flip test
//     TRANSPORT_SELF_TEST_9,    // CRC repalced 
//     TRANSPORT_SELF_TEST_10,   // DATA random bit fliped  
//     TRANSPORT_SELF_TEST_11,    // DATA random byte changed
//     MAIN_SELF_TEST_1,         // selftest watchdog timer check
// };

enum class TEST_OPERATION{
    IDLE,  
    TRANSPORT_GOOD_PACKET,
    TRANSPORT_STRESS_PACKET,
    TRANSPORT_TYPE_CHANGE,
    TRANSPORT_ACK_CHANGE,
    TRANSPORT_ID_CHANGE,
    TRANSPORT_DLC_CHANGE,
    TRANSPORT_DLC_OVERFLOW,
    TRANSPORT_CRC_BIT_FLIP,
    TRANSPORT_CRC_RANDOM_CHANGE,
    TRANSPORT_DATA_BIT_FLIP,
    TRANSPORT_DATA_BYTE_CHANGE,
    DIAGNOSTIC_WDT_TIMEOUT,
    TEST_RESULT_PASSED, 
    TEST_RESULT_FAILED, 
    END_SELF_TEST
};

extern TEST_OPERATION self_test_state; 

//****************************************************


//******************** variables *********************

struct SelfTestContext {
    uint32_t next_transmission_time;
    uint32_t test_end_countdown_timer;

    uint8_t requested_test_id;
    uint8_t current_active_test;  // this must be set false on pass/ fail case 
    TEST_RETURN_STATUS current_test_status_code;
    
    uint8_t current_test_packet;

    bool diagnostics_active;
    bool test_end_counter;
    bool watchdog_timer_test_active;

    TEST_OPERATION state; // deleted enum 
    TEST_OPERATION active_test;  // new 
    TEST_ID new_requested_test_id;
    TEST_ID new_current_active_test;
};

namespace self_test {
  extern SelfTestContext ctx;
}

//****************************************************

#endif 