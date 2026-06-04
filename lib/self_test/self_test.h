#ifndef SELF_TEST_H
#define SELF_TEST_H

#pragma once

#include <Arduino.h>
#include "global_config.h"
#include "debug.h"

//=============== test status codes ================
// enum class SELF_TEST_RETURN_CODE{ 
//     NO_TEST_RUNNING, 
//     TEST_RUNNING, 
//     TEST_PASSED, 
//     TEST_FAILED,
//     TEST_COMPLETED
// }; 

#define SELFTEST_NO_TEST_RUNNING                0
#define SELFTEST_RUNNING                        1
#define SELFTEST_PASSED                         2
#define SELFTEST_FAILED                         3 
#define SELFTEST_COMPLETED                      4
// //================ ================ ================ 

constexpr uint8_t ST_WDT_EXTRA_TIME =           250;   // tune to stop timout errors 
//=============== gloabal variables ================

//================ ================ ================ 

//====================== API function calls  ======================
uint8_t run_test_case();
bool watchdog_test_active(); 
bool request_self_test(uint8_t test_id);   // call from main to acivate a test must have state macheine running.  

#endif
