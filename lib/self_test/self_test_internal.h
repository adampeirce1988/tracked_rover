#ifndef SELF_TEST_INTERNAL_H
#define SELF_TEST_INTERNAL_H

#include <stdint.h>
#include "debug.h"


//********** random percentage congifg **********
#define RANDOM_ACK_PERCENTAGE             20 //% chance 
#define RANDOM_DELAY_PERCENTAGE           20 //% chance
//***********************************************

//**************** test 1 config **************** 
// normal transmission test moderate transmision rate mixed frames
#define TEST_1_PACKET_COUNT               50
#define TEST_1_PACKET_DELAY_US            15000 
#define TEST_END_COUNTDOWN_TIMER_US       500000    // 0.5 seconds
#define TEST_1_RANDOM_DELAY               false
//*********************************************** 

//**************** test 2 config **************** 
// stress test rapid back to back random packets inc acks 
#define TEST_2_PACKET_COUNT               50
#define TEST_2_PACKET_DELAY               5000
#define TEST_2_END_COUNTDONW_TIMER_US     500000
#define TEST_2_RANDOM_DELAY               true
//************************************************

//**************** test 3 config ***************** 
//************************************************
//**************** test 4 config ***************** 
//************************************************
//**************** test 5 config ***************** 
//************************************************

//**************** test 6 config **************** 
// stress test rapid back to back random packets inc acks 
#define TEST_3_PACKET_COUNT               50
#define TEST_3_PACKET_DELAY               5000
#define TEST_3_RANDOM_DELAY               true
#define TEST_3_END_COUNTDONW_TIMER_US     500000
//************************************************

//********* internal function prototypes ************
void check_self_test_resutls(uint8_t status_code);
uint8_t weighted_random_ack();
uint16_t weighted_random_delay(uint16_t std_delay); 
uint8_t self_test_1(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active);
uint8_t self_test_6(); 
//****************************************************


//***************** test enums class ***************** 
enum class TEST_OPERATION{ 
    TEST_DISPACHER, 
    TEST_RESULT_PASSED, 
    TEST_RESULT_FAILED, 
    END_SELF_TEST, 
    TRANSPORT_SELF_TEST_1, 
    TRANSPORT_SELF_TEST_2, 
    TRANSPORT_SELF_TEST_3,
    MAIN_SELF_TEST_1
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

