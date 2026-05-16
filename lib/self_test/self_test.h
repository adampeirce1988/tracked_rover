#ifndef SELF_TEST_H
#define SELF_TEST_H

#include <Arduino.h>
#include "global_config.h"
#include "debug.h"

//=============== test status codes ================ 
#define SELFTEST_NO_TEST_RUNNING     0
#define SELFTEST_RUNNING             1
#define SELFTEST_PASSED              2
#define SELFTEST_FAILED              3 
#define SELFTEST_COMPLETED           4

// //================ ================ ================ 

// #define RANDOM_ACK_PERCENTAGE        20 //% chance 
// #define RANDOM_DELAY_PERCENTAGE      20 //% chance

// //**************** test 1 config **************** 
// // normal transmission test moderate transmision rate mixed frames
// #define TEST_1_PACKET_COUNT          50
// #define TEST_1_PACKET_DELAY_US       10000 
// #define TEST_1_RANDOM_DELAY          false
// //*********************************************** 

// //**************** test 2 config **************** 
// // stress test rapid back to back random packets inc acks 
// #define TEST_2_PACKET_COUNT          50
// #define TEST_2_PACKET_DELAY          5000
// #define TEST_2_RANDOM_DELAY          false
// //************************************************

// //**************** test 3 config **************** 
// // stress test rapid back to back random packets inc acks 
// #define TEST_3_PACKET_COUNT          50
// #define TEST_3_PACKET_DELAY          5000
// #define TEST_3_RANDOM_DELAY          true
// //************************************************


//====================== gloabal variables ======================

//bool diagnostics_active = true; // sent via call to diagnostics once complete //moved to self_test.cpp


//====================== API function calls  ======================

uint8_t run_test_case();
bool request_self_test(uint8_t test_id);   // call from main to acivate a test must have state macheine running.  

#endif
