#ifndef SELF_TEST_INTERNAL_H
#define SELF_TEST_INTERNAL_H

#include <stdint.h>
#include "debug.h"
#include "transport.h"

//************* global test congifg *************
constexpr uint32_t TEST_END_COUNTDOWN_TIMER_US =     500000;   // 0.5 seconds
constexpr bool TEST_1_VERBOUS_OUTPUT =               true; 
constexpr bool TEST_2_VERBOUS_OUTPUT =               false; 
constexpr bool TEST_6_VERBOUS_OUTPUT =               true; 
//***********************************************

//********** random percentage congifg **********
constexpr uint8_t RANDOM_ACK_PERCENTAGE =            20; //% chance 
constexpr uint8_t RANDOM_DELAY_PERCENTAGE =          20; //% chance
//***********************************************

//**************** test 1 config **************** 
// normal transmission test moderate transmision rate mixed frames
constexpr uint8_t TEST_1_PACKET_COUNT =              100;
constexpr uint16_t TEST_1_PACKET_DELAY_US =          15000; 
constexpr bool TEST_1_RANDOM_DELAY =                 false;
//*********************************************** 

//**************** test 2 config **************** 
// stress test rapid back to back random packets inc acks 
//with random timing between packets 
constexpr uint8_t TEST_2_PACKET_COUNT =              100;
constexpr uint16_t TEST_2_PACKET_DELAY =             5000;
constexpr bool TEST_2_RANDOM_DELAY =                 true;
//************************************************

//**************** test 3 config ***************** 
// CRC error injection
constexpr uint8_t TEST_3_PACKET_COUNT =              100;
constexpr uint16_t TEST_3_PACKET_DELAY_US =          20000;
constexpr uint8_t TEST_3_ERROR_COUNT =               TEST_3_PACKET_COUNT / 10;
constexpr uint8_t TEST_3_ERROR_VALUE =               0xFE; 
//*********************************************** 

//**************** test 4 config ***************** 
// CRC random bit flip error
constexpr uint8_t TEST_4_PACKET_COUNT =              100;
constexpr uint16_t TEST_4_PACKET_DELAY_US =          20000;
constexpr uint8_t TEST_4_ERROR_COUNT =               TEST_3_PACKET_COUNT / 10;
//************************************************

//**************** test 5 config ***************** 
// test not yet implimented test to check the opperation of the RX_WDT and recovery 
//************************************************

//**************** test 6 config **************** 
// stress test rapid back to back random packets inc acks 
constexpr uint8_t TEST__PACKET_COUNT =              50;
constexpr uint16_t TEST_6_PACKET_DELAY =            5000;
constexpr bool TEST_6_RANDOM_DELAY =                true;
//************************************************

//********* internal function prototypes ************
void check_self_test_resutls(uint8_t status_code);
uint8_t weighted_random_ack();
uint16_t weighted_random_delay(uint16_t std_delay); 
uint8_t self_test_inline_crc_calc(uint8_t CRC, uint8_t byte);
uint8_t self_test_1(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active);
uint8_t self_test_2(uint8_t no_of_packets, uint16_t delay_time_us, uint8_t error_count, TX_SET_FAULT_MODE fault_type, uint8_t fault_value = 0);
uint8_t self_test_6(); 

//****************************************************


//***************** test enums class ***************** 
enum class TEST_OPERATION{ 
    TEST_DISPACHER, 
    TEST_RESULT_PASSED, 
    TEST_RESULT_FAILED, 
    END_SELF_TEST, 
    TRANSPORT_SELF_TEST_1,   // good packet test 
    TRANSPORT_SELF_TEST_2,   // good packet stress test
    TRANSPORT_SELF_TEST_3,   // CRC repalced error check 
    TRANSPORT_SELF_TEST_4,   // CRC crc bit changed check 
    MAIN_SELF_TEST_1         // selftest watchdog timer check
};

extern TEST_OPERATION self_test_state; 
//****************************************************


//******************** variables *********************
namespace self_test {
    extern uint32_t next_transmission_time;
    extern uint32_t test_end_countdown_timer;
    extern uint8_t requested_test_id; 
    extern uint8_t current_active_test;  
    extern uint8_t current_test_packet;
    extern uint8_t current_test_status_code;
    extern bool diagnostics_active; 
    extern bool test_end_counter;  
    extern bool watchdog_timer_test_active; 
}
//****************************************************


#endif 

