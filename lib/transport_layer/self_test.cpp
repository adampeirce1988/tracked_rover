
//////////////////////////////////////////////////////////////////////////////
//-------------------- BRANCH -self test implimnetation --------------------//
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "log.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"


#define DEBUG_FILE DBG_SELF_TEST 


//================ fuction prototypes ================= 
uint8_t weighted_random_ack();
//================= ================= =================

enum class TEST_OPERATION{ 
    TEST_DISPACHER, 
    TEST_RESULT_PASSED, 
    TEST_RESULT_FAILED, 
    END_SELF_TEST, 
    TRANSPORT_SELF_TEST_1, 
    TRANSPORT_SELF_TEST_2 
}; // test state belonged to this 

//===============  variables =============== 
namespace self_test {
    uint32_t next_transmission_time= 0;
    uint8_t current_packet = 0; 
    uint8_t requested_test_id = 0; 
    uint8_t current_status; 
    bool test_in_progress = false;  // testing only


    // test 1 
    uint8_t requested_no_of_packets = 0; 
    uint32_t delay_time = 0; 
    uint8_t current_test_packet = 0;

    TEST_OPERATION self_test_state = TEST_OPERATION::TEST_DISPACHER; 

}

bool diagnostics_active = true; // sent via call to diagnostics once complete

extern struct selftest_metrics transport_log;


//=============  ============= =============
void request_self_test(uint8_t test_id){
    self_test::requested_test_id = test_id; 
}

bool request_self_test_1( uint8_t no_of_packets, uint32_t delay_time_us){
    self_test::requested_no_of_packets = no_of_packets;
    self_test::delay_time = delay_time_us; 
    self_test::current_test_packet = 0; // reset this on test call 
    set_transport_selftest_loging_active();
    transpoert_log_clear();

    //test_state = TRANSPORT_SELF_TEST_1; 
    return true;  // added to clear an error. 
}

bool self_test_1(uint8_t current_packet, uint8_t no_of_packets, uint32_t delay_time_us){
    
    // print information (only at the sart of the test)
    if(current_packet == 0){
        transport_set(&fifo_io); // set current transport to fifo. 

        self_test::next_transmission_time = micros();
        
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
    }

    // create a loop to only send packts after alocatred time. 
    if(micros() > self_test::next_transmission_time){
        self_test::next_transmission_time += delay_time_us;

        for(uint8_t i = 0; i < no_of_packets; i ++){
            uint8_t type = random(1, 256);           // random type 1-255
            uint8_t ack  = weighted_random_ack();    // weighted ack 20% chance of ack 
            uint8_t dlc  = random(0,7);              // set random dlc
            uint8_t data[6]; 
            for(int i = 0; i < dlc; i++){
                data[i] = random(0,255);
            }

            // transmit packet
            transport_queue_message(type, ack, dlc, data);

            //increment current packet
            self_test::current_test_packet ++; 
            
            // handle end of test.
            if(i == no_of_packets){ 
                // reset current packet
                self_test::current_test_packet = 0; 

                // change this to retun a result if no errors are loged if errors are loged then return an error reprot.
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "self_test_1 completed result: PASS"); // change to return

                // set transport back to uart on completion of test. 
                transport_set(&uart_io);

                return SELFTEST_PASSED; 
            }
            else{ 
                return SELFTEST_RUNNING;
            }
        }  
    } 
    return SELFTEST_RUNNING; 
}

uint8_t weighted_random_ack(){

    uint8_t r = random(0, 100); // random no 0 - 99 

    if(r < 20)  {return ACK_REQUEST;}  // 20% chance of requiring ack 
    return NORMAL_FRAME;               // 80% chance of normal frame 
}


///////////////////////////////////////////////////////////////////////////////
//                          SELF TEST STATE MACHINE                          
//----------------------------------------------------------------------------
//  ENTRY POINT: SELF TEST EXECUTION START
//  This section handles all diagnostic / transport validation tests.
//  Control flow is managed via TEST_OPERATION state machine.
//  All test results are logged via transport diagnostics system.
////////////////////////////////////////////////////////////////////////////////

uint8_t run_test_case(){

    switch(self_test::self_test_state){

        case TEST_OPERATION::TEST_DISPACHER:
            if(self_test::requested_test_id == 0){
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TEST", "no test case requested diagnostics exited without comopleting any test. requested_test_id: ", self_test::requested_test_id); 
            }
            else if(self_test::requested_test_id == 1){self_test::self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_1;}
            else if(self_test::requested_test_id == 2){self_test::self_test_state = TEST_OPERATION::TRANSPORT_SELF_TEST_2;}
            self_test::requested_test_id = 0; // rest the test request once the state is changed 
        break; 

        case TEST_OPERATION::TEST_RESULT_PASSED:
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_1: passed");
            self_test::self_test_state = TEST_OPERATION::END_SELF_TEST; 
            return self_test::current_status;
        break;

        case TEST_OPERATION::TEST_RESULT_FAILED:
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_1: failed");
            self_test::self_test_state = TEST_OPERATION::END_SELF_TEST;
            return self_test::current_status;  
        break;

        case TEST_OPERATION::END_SELF_TEST:
            // code to close the self test gos here
            self_test::current_status = SELFTEST_ENDED; 
            self_test::self_test_state = TEST_OPERATION::TEST_DISPACHER;
            return self_test::current_status; 
        break;

        /////////////////////////////// TEST 1 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_1: 
        self_test::test_in_progress = self_test_1(self_test::current_test_packet, 50, 2500); // send 50 good packkets at 25ms intervals. 

        if(self_test::current_status == SELFTEST_PASSED){
            set_transport_selftest_loging_inactive(); 
            PRINT_TRANSPORT_SELFTEST_LOG();
            //self_test_state = TEST_OPERATION::TEST_RESULT_PASSED;
        }
        else if(self_test::current_status == SELFTEST_FAILED){
            set_transport_selftest_loging_inactive(); 
            PRINT_TRANSPORT_SELFTEST_LOG();
            //self_test_state = TEST_OPERATION::TEST_RESULT_FAILED;
        }
        else{
            return self_test::current_status;
        }
        // asses the test results for pass or fail. retuen a value based on the out come. 
        // set test_status inactive 
     
        // END OF TEST 1
        break; 
       
        /////////////////////////////// TEST 2 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_2: 
          // test 2 code here. 
          //test_state = TEST_OPERATION::TEST_RESULT_PASSED;
        break; 
    }
    return ST_RETURN_NOT_HIT; 
}