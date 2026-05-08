
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


//===============  variables =============== 
uint32_t next_transmission_time= 0;
uint8_t current_packet = 0; 
uint8_t test_request = 0; 
uint8_t current_status; 


// test 1 
uint8_t requested_no_of_packets = 0; 
uint32_t delay_time = 0; 
uint8_t current_test_packet = 0;


bool diagnostics_active = true; // sent via call to diagnostics once complete

extern struct selftest_metrics transport_log;


//=============  ============= =============
void request_self_test(uint8_t test_id){
    test_request = test_id; 
}

bool request_self_test_1( uint8_t no_of_packets, uint32_t delay_time_us){
    requested_no_of_packets = no_of_packets;
    delay_time = delay_time_us; 
    current_test_packet = 0; // reset this on test call 
    set_transport_selftest_active();
    transpoert_log_clear();

    //test_state = TRANSPORT_SELF_TEST_1; 
    return true;  // added to clear an error. 
}

bool self_test_1(uint8_t current_packet, uint8_t no_of_packets, uint32_t delay_time_us){
    
    // print information (only at the sart of the test)
    if(current_packet == 0){
        transport_set(&fifo_io); // set current transport to fifo. 

        next_transmission_time = micros();
        
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
    }

    // create a loop to only send packts after alocatred time. 
    if(micros() > next_transmission_time){
        next_transmission_time += delay_time_us;

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
            current_test_packet ++; 
            
            // handle 
            if(i == no_of_packets){ 
                // reset current packet
                current_test_packet = 0; 

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

enum test_opperations{ 
    TEST_DISPACHER, 
    TRANSPORT_SELF_TEST_1, 
    TRANSPORT_SELF_TEST_2, 
    TEST_RESULT_PASSED, 
    TEST_RESULT_FAILED, 
    END_SELF_TEST 
} test_state;

 

uint8_t run_test_case(){

    switch(test_state){

        case TEST_DISPACHER:
            if(test_request == 0){break;}
            else if(test_request == 1){test_state = TRANSPORT_SELF_TEST_1;}
            else if(test_request == 2){test_state = TRANSPORT_SELF_TEST_2;}
            test_request = 0; // rest the test request once the state is changed 
        break; 

        case TRANSPORT_SELF_TEST_1: 

        current_status = self_test_1(current_test_packet, 50, 2500); // send 50 good packkets at 25ms intervals. 

        if(current_status == SELFTEST_PASSED){
            PRINT_TRANSPORT_SELFTEST_LOG();
            test_state = TEST_RESULT_PASSED;
        }
        else if(current_status == SELFTEST_FAILED){
            PRINT_TRANSPORT_SELFTEST_LOG();
            test_state = TEST_RESULT_FAILED;
        }
        // asses the test results for pass or fail. retuen a value based on the out come. 
        // set test_status inactive 
        set_transport_selftest_inactive(); 
        // END OF TEST 1
        break; 
       
        case TRANSPORT_SELF_TEST_2: 
          // test 2 code here. 
          test_state = TEST_RESULT_PASSED;
        break; 

        case TEST_RESULT_PASSED:
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_1: passed");
            test_state = END_SELF_TEST; 
            return current_status;
        break;

        case TEST_RESULT_FAILED:
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_1: failed");
        test_state = END_SELF_TEST;
        return current_status;  
        break;

        case END_SELF_TEST:
            // code to close the self test gos here
            current_status = SELFTEST_ENDED; 
            test_state = TEST_DISPACHER;
            return current_status; 
        break;

    }

    return ST_RETURN_NOT_HIT; 
}