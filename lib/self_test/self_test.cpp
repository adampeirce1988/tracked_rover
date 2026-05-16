
//////////////////////////////////////////////////////////////////////////////
//-------------------- BRANCH -self test implimnetation --------------------//
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "log.h"
#include "self_test.h"
#include "self_test_internal.h"
#include "debug.h"
#include "debug_config.h"



#define DEBUG_FILE DBG_SELF_TEST 


//================ fuction prototypes ================= 
//uint8_t weighted_random_ack();
//uint16_t weighted_random_dealy(uint16_t std_delay); 
bool analise_test_results(uint8_t expected_resutlts[]);
//uint8_t self_test_1(uint8_t current_packet, uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active);

//================= ================= =================

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

//===============  variables =============== 
namespace self_test {
    uint32_t next_transmission_time= 0;
    uint8_t current_packet = 0; 
    uint8_t requested_test_id = 0; 
    uint8_t current_active_test = 0;  
    //bool test_in_progress = false;  // testing only
    uint8_t current_test_status_code = 0; // testing only

    bool diagnostics_active = false; 

    // test 1 
    uint8_t requested_no_of_packets = 0; 
    uint32_t delay_time = 0; 
    uint8_t current_test_packet = 0;

    //TEST_OPERATION self_test_state = TEST_OPERATION::TEST_DISPACHER; 

}

TEST_OPERATION self_test_state = TEST_OPERATION::TEST_DISPACHER; 

//bool diagnostics_active = true; // sent via call to diagnostics once complete ** REMOVE if CODE COMPILES ** 


//extern struct selftest_metrics transport_log;

//=============  ============= =============
bool request_self_test(uint8_t test_id){
    self_test::requested_test_id = test_id; 
    return true; 
}

bool analise_test_results(uint8_t expected_resutlts[] ){ //******* NEEDS TO BE COMPLETED *******
    if(true){
        return true; 
    }
    else{
        return false;
    }
}

// uint8_t self_test_1(uint8_t current_packet, uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active){
    
//     // test setup(runs only once)
//     if(current_packet == 0){

//         transport_set(&fifo_io);                  // set current transport to fifo. 
//         transport_selftest_log_clear();           // reset all previous loged data.  *** MAY NEED TAYLORING TO SPECIFIC TEST TYPES **
//         set_transport_selftest_loging_active();   // activate loging

//         self_test::next_transmission_time = micros();
//         // print information (only at the sart of the test)
//         DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
//         DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
//     }

//     // create a non-blocking loop to only send packts after alocatred time. 
//     if(micros() > self_test::next_transmission_time){
        
//         // set the next transmision time
//         self_test::next_transmission_time += delay_time_us;
//         // reduce time by a randon amount if random delay is requested.

//         if(random_delay_active == true){
//             //self_test::next_transmission_time -= weighted_random_dealy(delay_time_us); 
//         }
    
//         //build random packet 
//         if(current_packet < (no_of_packets)){
            
//             DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", "packets sent current packet count: ", current_packet);

//             uint8_t type = random(1, 256);                    // random type 1-255 ** NO TYPE CHECK IMPLIMENTED **
//             uint8_t ack  = weighted_random_ack();             // weighted ack 20% chance of ack 
//             uint8_t dlc  = random(0,(MAX_PAYLOAD_LEN + 1));   // set random dlc
//             uint8_t data[MAX_PAYLOAD_LEN]; 
//             for(int i = 0; i < dlc; i++){
//                 data[i] = random(0,255);
//             }

//             // report when an  ack is sent. 
//             if(ack == ACK_REQUEST){
//                 DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "ACK", "random packet sent with ack request set. ack_request: ", ack);
//             }

//             // transmit packet
//             transport_queue_message(type, ack, dlc, data);

//             //increment current packet
//             self_test::current_test_packet ++; 
//         }
            
//         // runs one at the end of the test
//         if(current_packet == (no_of_packets)){ 
//             // reset current packet
//             self_test::current_test_packet = 0; 

//             // change this to retun a result if no errors are loged if errors are loged then return an error reprot.
//             DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TEST", "self_test_1 completed result: UNKNOWN"); // change to return

            
//             //transport_set(&uart_io); *** DISABLED FOR TESTING *** // set transport back to uart on completion of test.
//             set_transport_selftest_loging_inactive();               // dissable loging once test is completed 
            

//             // check for pass or fail 0 errors expected. 
//             if(transport_test_log.total_errors == 0 && transport_test_log.packets_received == no_of_packets){ // change to a function call that will accept ans expectd test result array. 
//                 return SELFTEST_PASSED; // assess test results and mark pass/ fail flag 
//             }
//             else{
//                 return SELFTEST_FAILED;
//             }
            
//         }
       
//     } 
//     return SELFTEST_RUNNING; 
//}


uint8_t weighted_random_ack(){

    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_ACK_PERCENTAGE)  {return ACK_REQUEST;}  // 20% chance of requiring ack 
    return NORMAL_FRAME;               // 80% chance of normal frame 
}

uint16_t weighted_random_delay(uint16_t std_delay){
   
    uint8_t r = random(0, 101); // random no 0 - 100 

    if(r < RANDOM_DELAY_PERCENTAGE){
        uint16_t one_percent = std_delay / 100; 
        uint16_t delay_reduction = random((one_percent * 20), (one_percent * 35)); 
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "WRD", "Weighted randon delay value: ", delay_reduction, "us.");
        return delay_reduction; 
    }
    else{
        return 0; 
    }
}

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
            self_test_state = TEST_OPERATION::TEST_DISPACHER;
            return SELFTEST_COMPLETED;
        break;

        /////////////////////////////// TEST 1 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_1: 
            // run the self test and log the status retuend 
            self_test::current_test_status_code = self_test_1(self_test::current_test_packet, TEST_1_PACKET_COUNT, TEST_1_PACKET_DELAY_US, TEST_1_RANDOM_DELAY); 

            if(self_test::current_test_status_code == SELFTEST_PASSED){
                set_transport_selftest_loging_inactive(); 
                PRINT_TRANSPORT_SELFTEST_LOG();
                self_test_state = TEST_OPERATION::TEST_RESULT_PASSED;
                
            }
            else if(self_test::current_test_status_code == SELFTEST_FAILED){
                set_transport_selftest_loging_inactive(); 
                PRINT_TRANSPORT_SELFTEST_LOG();
                self_test_state = TEST_OPERATION::TEST_RESULT_FAILED;
            }
        
            return self_test::current_test_status_code;
        break; 
       
        /////////////////////////////// TEST 2 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_2: 
             // run the self test and log the status retuend 
            self_test::current_test_status_code = self_test_1(self_test::current_test_packet, TEST_1_PACKET_COUNT, TEST_1_PACKET_DELAY_US, TEST_2_RANDOM_DELAY); 

            if(self_test::current_test_status_code == SELFTEST_PASSED){
                set_transport_selftest_loging_inactive(); 
                PRINT_TRANSPORT_SELFTEST_LOG();
                self_test_state = TEST_OPERATION::TEST_RESULT_PASSED;
            }
            else if(self_test::current_test_status_code == SELFTEST_FAILED){
                set_transport_selftest_loging_inactive(); 
                PRINT_TRANSPORT_SELFTEST_LOG();
                self_test_state = TEST_OPERATION::TEST_RESULT_FAILED;
            }
        
            return self_test::current_test_status_code;
        break; 
        
        /////////////////////////////// TEST 3 ///////////////////////////////
        case TEST_OPERATION::TRANSPORT_SELF_TEST_3:
                // create test 3 here test 3 shoulsd include random back to back packets 
        break; 

                /////////////////////////////// TEST 6 ///////////////////////////////
        case TEST_OPERATION::MAIN_SELF_TEST_1:
                // test to check the WDT timeout in main this needs a test flag to get a pass result otherwise an error will be dispalyed on failure. 
                // timeout behavior should still remain stable and not log a hard crash/ lockup. 
        break; 
    }

    return SELFTEST_NO_TEST_RUNNING;
}