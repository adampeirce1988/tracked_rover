
#include <Arduino.h>
#include "self_test_internal.h"
#include "global_config.h"
#include "transport.h"
#include "self_test.h" 
#include "debug.h"
#include "log.h"


#define DEBUG_FILE DBG_SELF_TEST 

//====================== variables ====================== 
namespace self_test {
    uint32_t next_transmission_time= 0;
    uint32_t test_end_countdown_timer = 0;  
    uint8_t requested_test_id = 0; 
    uint8_t current_active_test = 0;  
    uint8_t current_test_status_code = 0;
    uint8_t current_test_packet = 0; 
    bool diagnostics_active = false; 
    bool test_end_counter = false; 
    bool watchdog_timer_test_active = false; 
}

TEST_OPERATION self_test_state = TEST_OPERATION::TEST_DISPACHER; 
//=========================================================


//===================== External APIs =====================
bool request_self_test(uint8_t test_id){
    self_test::requested_test_id = test_id; 
    return true; 
}

bool watchdog_test_active(){
    if(self_test::watchdog_timer_test_active == true){
        return true; 
    }
    else{
        return false;
    }
}
//=========================================================
; 

///////////////////////////////////////////////////////////////////////////////
//                          SELF TEST STATE MACHINE                          
//----------------------------------------------------------------------------
//  ENTRY POINT: SELF TEST EXECUTION START
//  This section handles all diagnostic / transport validation tests.
//  Control flow is managed via TEST_OPERATION state machine.
//  All test results are logged via transport diagnostics system.
//
//  All self test descritons can be fond in self_test.txt *** NOT YET IMPLIMENTED ***
////////////////////////////////////////////////////////////////////////////////

uint8_t run_test_case(){

    switch(self_test_state){

        case TEST_OPERATION::TEST_DISPACHER:

            if(self_test::requested_test_id == 0){
                self_test::current_test_status_code = 0; // no test in progress
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TEST", "no test case requested diagnostics exited without comopleting any test. requested_test_id: ", self_test::requested_test_id); 
            }
            else if(self_test::requested_test_id == 1){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_1;}
            else if(self_test::requested_test_id == 2){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_2;}
            else if(self_test::requested_test_id == 2){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_3;}
            else if(self_test::requested_test_id == 3){self_test_state = TEST_OPERATION::MAIN_SELF_TEST_1;}

            // rest the test request once the state is changed 
            self_test::current_active_test = self_test::requested_test_id; 
            self_test::requested_test_id = 0;
        break; 

        case TEST_OPERATION::TEST_RESULT_PASSED:
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_", self_test::current_active_test, ": PASSED.");
            self_test_state = TEST_OPERATION::END_SELF_TEST; 
        break;

        case TEST_OPERATION::TEST_RESULT_FAILED:
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_", self_test::current_active_test, ": FAILED.");
            self_test_state = TEST_OPERATION::END_SELF_TEST;
        break;

        case TEST_OPERATION::END_SELF_TEST:
            // code to close the self test gos here state not required
            // always rest the sys variables on exit
            set_transport_selftest_loging_inactive();
            self_test_state = TEST_OPERATION::TEST_DISPACHER;
            return SELFTEST_COMPLETED;
        break;

        /////////////////////////////// TEST 1 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_1: 
            // run the self test and log the status retuend 
            self_test::current_test_status_code = self_test_1(TEST_1_PACKET_COUNT, TEST_1_PACKET_DELAY_US, TEST_1_RANDOM_DELAY); 
            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code;
        break; 
       
        /////////////////////////////// TEST 2 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_2: 
             // run the self test and log the status retuend 
            self_test::current_test_status_code = self_test_1(TEST_1_PACKET_COUNT, TEST_1_PACKET_DELAY_US, TEST_2_RANDOM_DELAY); 
            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code;
        break; 
        
        /////////////////////////////// TEST 3 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_3:
                // create test 3 here test 3 shoulsd include random back to back packets 
                check_self_test_resutls(self_test::current_test_status_code); 
                return self_test::current_test_status_code; 
        break; 
        
        /////////////////////////////// TEST 4 ///////////////////////////////
        /////////////////////////////// TEST 5 ///////////////////////////////

        /////////////////////////////// TEST 6 ///////////////////////////////
        case TEST_OPERATION::MAIN_SELF_TEST_1:
                self_test::current_test_status_code = self_test_6();
                check_self_test_resutls(self_test::current_test_status_code); 
                return self_test::current_test_status_code; 
        break;


    }
    return SELFTEST_NO_TEST_RUNNING;
}