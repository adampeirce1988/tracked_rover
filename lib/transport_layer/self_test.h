#ifndef SELF_TEST_H
#define SELF_TEST_H

#include <Arduino.h>
#include "global_config.h"
#include "debug.h"

//=============== test status codes ================ 
#define SELFTEST_RUNNING             1
#define SELFTEST_PASSED              2
#define SELFTEST_FAILED              3 
#define SELFTEST_ENDED               4
#define ST_RETURN_NOT_HIT            5
//================ ================ ================ 

//====================== gloabal variables ======================

//bool diagnostics_active = true; // sent via call to diagnostics once complete //moved to self_test.cpp


//====================== API function calls  ======================

uint8_t run_test_case();
bool request_test_case(uint8_t test_id);   // this is not implimented anywhere. in main this needs to return to a var that will activate diag on true. 
bool self_test_1(uint8_t no_of_packets, uint8_t delay_time_us);

#endif
