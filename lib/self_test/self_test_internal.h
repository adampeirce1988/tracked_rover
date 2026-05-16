#ifndef SELF_TEST_INTERNAL_H
#define SELF_TEST_INTERNAL_H

#include <stdint.h>
#include "debug.h"


//********** random percentage congifg **********
#define RANDOM_ACK_PERCENTAGE        20 //% chance 
#define RANDOM_DELAY_PERCENTAGE      20 //% chance
//***********************************************

//**************** test 1 config **************** 
// normal transmission test moderate transmision rate mixed frames
#define TEST_1_PACKET_COUNT          50
#define TEST_1_PACKET_DELAY_US       10000 
#define TEST_1_RANDOM_DELAY          false
//*********************************************** 

//**************** test 2 config **************** 
// stress test rapid back to back random packets inc acks 
#define TEST_2_PACKET_COUNT          50
#define TEST_2_PACKET_DELAY          5000
#define TEST_2_RANDOM_DELAY          false
//************************************************

//**************** test 3 config **************** 
// stress test rapid back to back random packets inc acks 
#define TEST_3_PACKET_COUNT          50
#define TEST_3_PACKET_DELAY          5000
#define TEST_3_RANDOM_DELAY          true
//************************************************

//********* internal function prototypes ***********
uint8_t weighted_random_ack();
uint16_t weighted_random_dealy(uint16_t std_delay); 
//bool analise_test_results(uint8_t expected_resutlts[]);
uint8_t self_test_1(uint8_t current_packet, uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active);
//**************************************************


namespace self_test {
    extern uint32_t next_transmission_time;
    extern uint8_t current_packet; 
    extern uint8_t requested_test_id; 
    extern uint8_t current_active_test;  

    extern uint8_t current_test_status_code; // testing only

    extern bool diagnostics_active; 

    // test 1 
    extern uint8_t requested_no_of_packets; 
    extern uint32_t delay_time; 
    extern uint8_t current_test_packet;

}


#endif 

