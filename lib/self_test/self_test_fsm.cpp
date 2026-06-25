
#include <Arduino.h>
#include "self_test_internal.h"
#include "global_config.h"
#include "transport.h"
#include "self_test.h" 
#include "debug.h"


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
};

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

        case TEST_OPERATION::TEST_DISPACHER:{

            if(self_test::requested_test_id == 0){
                self_test::current_test_status_code = 0; // no test in progress
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TEST", "no test case requested diagnostics exited without comopleting any test. requested_test_id: ", self_test::requested_test_id); 
            }
            else if(self_test::requested_test_id == 1){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_1;}
            else if(self_test::requested_test_id == 2){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_2;}
            else if(self_test::requested_test_id == 3){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_3;}
            else if(self_test::requested_test_id == 4){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_4;}
            else if(self_test::requested_test_id == 5){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_5;}
            else if(self_test::requested_test_id == 6){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_6;}
            else if(self_test::requested_test_id == 7){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_7;}
            else if(self_test::requested_test_id == 8){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_8;}
            else if(self_test::requested_test_id == 9){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_9;}
            else if(self_test::requested_test_id == 10){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_10;}
            else if(self_test::requested_test_id == 11){self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_11;}
            else if(self_test::requested_test_id == 12){self_test_state = TEST_OPERATION::MAIN_SELF_TEST_1;}

            // rest the test request once the state is changed 
            self_test::current_active_test = self_test::requested_test_id; 
            self_test::requested_test_id = 0;
        } break; 

        // Should there be a case START_SELF_TEST: ?? **********************

        case TEST_OPERATION::TEST_RESULT_PASSED:{
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_", self_test::current_active_test, ": PASSED.");
            self_test_state = TEST_OPERATION::END_SELF_TEST; 
        } break;

        case TEST_OPERATION::TEST_RESULT_FAILED:{
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_", self_test::current_active_test, ": FAILED.");
            self_test_state = TEST_OPERATION::END_SELF_TEST;
        } break;

        case TEST_OPERATION::END_SELF_TEST:{
            // code to close the self test goes here; state not required
            // always reset the sys variables on exit
     
            //st_logging_inactive();  // set ST logging inactive here 
            
            self_test_state = TEST_OPERATION::TEST_DISPACHER;
            return SELFTEST_COMPLETED;
        } break;

        /////////////////////////////// TRANSPORT TEST 1 ///////////////////////////////
        // Run a good packet test with moderate timing 
        case TEST_OPERATION::TRANSPORT_SELF_TEST_1:{

            self_test::current_test_status_code = self_test_transport_random_packet(
                TEST_1_PACKET_COUNT, 
                STANDARD_PACKET_DEALY_US, 
                DISABLE_RANDOM_PACKET_TIMING_DELAY
            ); 

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code;
        } break; 
       
        /////////////////////////////// TRANSPORT TEST 2 ///////////////////////////////
         // run a good packet test with tight timing and random reduced time between transmisios (1:10)
        case TEST_OPERATION::TRANSPORT_SELF_TEST_2:{ 

            self_test::current_test_status_code = self_test_transport_random_packet(
                TEST_1_PACKET_COUNT, 
                STRESS_TEST_PACKET_DELAY_US, 
                ENABLE_RANDOM_PACKET_TIMING_DELAY
            ); 

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code;
        } break; 
        
        /////////////////////////////// TRANSPORT TEST 3 ///////////////////////////////
        // change the type wil rsult in a crc error
        case TEST_OPERATION::TRANSPORT_SELF_TEST_3:{

            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::TYPE_CHANGE
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 

        } break; 

        /////////////////////////////// TRANSPORT TEST 4 ///////////////////////////////
        // change ack the will result in a crc error
        case TEST_OPERATION::TRANSPORT_SELF_TEST_4:{

            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::ACK_CHANGE, 
                TRANSPORT_ACK_TYPE::NACK
            );
                
            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 
        } break;

        /////////////////////////////// TRANSPORT TEST 5 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_5:{
        // change the current packet id will result in a crc error
            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::ID_CHANGE
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 
        break; 
        }
        

        /////////////////////////////// TRANSPORT TEST 6 ///////////////////////////////
        // changes the DLC to a renadom valid int
        case TEST_OPERATION::TRANSPORT_SELF_TEST_6:{

            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DLC_CHANGE
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 
            break; 
        }
        
        /////////////////////////////// TRANSPORT TEST 7 ///////////////////////////////
        // DLC over max capactity test expeced early catch 
        case TEST_OPERATION::TRANSPORT_SELF_TEST_7:{
            
            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::DLC_OVER_MAX_CAPACITY
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 
            break; 
        }

        /////////////////////////////// TRANSPORT TEST 8 ///////////////////////////////
        // 1 bit in the CRC fliped randomly changed on 1 packet in 10.
        case TEST_OPERATION::TRANSPORT_SELF_TEST_8:{

            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::CRC_RAND_FLIP_BIT
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 
        }break; 
       

        /////////////////////////////// TRANSPORT TEST 9 ///////////////////////////////
        // good packet test with the CRC randomly changed on 1 packet in 10. 
        case TEST_OPERATION::TRANSPORT_SELF_TEST_9:{
            
            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT, 
                INJECTION_ERROR_COUNT, 
                TX_SET_FAULT_MODE::CRC_CHANGE,
                TEST_9_ERROR_VALUE
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 
        } break; 
    
 
        /////////////////////////////// TRANSPORT TEST 10 ///////////////////////////////
        //random data bit fliped expected crc error
        case TEST_OPERATION::TRANSPORT_SELF_TEST_10:{

            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DATA_FLIP_BIT
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code;
        }break;

        /////////////////////////////// TRANSPORT TEST 11 ///////////////////////////////
        //random data byte change expected crc error
        case TEST_OPERATION::TRANSPORT_SELF_TEST_11:{

            self_test::current_test_status_code = self_test_error_injection(
                INJECTION_PACKET_COUNT,
                INJECTION_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DATA_CHANGE
            );

            check_self_test_resutls(self_test::current_test_status_code); 
            return self_test::current_test_status_code; 

        } break; 
    

        /////////////////////////////// TEST 6 ///////////////////////////////
        case TEST_OPERATION::MAIN_SELF_TEST_1:{
                self_test::current_test_status_code = self_test_diagnostics_wdt();
                check_self_test_resutls(self_test::current_test_status_code); 
                return self_test::current_test_status_code; 
        }break;
    }
    
    return SELFTEST_NO_TEST_RUNNING;
}