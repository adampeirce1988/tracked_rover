#include <Arduino.h>
#include "global_config.h"
#include "transport.h"

#include "logger.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"
#include "messages.h"

#define DEBUG_FILE DBG_SELF_TEST

uint8_t next_injection_position = 0; 

uint8_t self_test_error_injection(uint8_t no_of_packets, uint8_t error_count, TX_SET_FAULT_MODE fault_type, uint8_t fault_value){

    // test setup(runs only once)
    if(self_test::current_test_packet == 0){

        //print test inoformartion // *** TODO: MAKE TESTING TYOE DYNAMIC AND REFLECT SELECTED TEST TYPE ***
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "running self_test_2 corrup crc error count: ", error_count, " corupt frames.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_1 tests packets transmitted speed: ", DEFAULT_PACKET_DELAY_US, " us");
        // enable progress bar  
        dissable_verbous_error();                 // disble verbous error reporting
        PRINT_PROGRESS_BAR_START();               // Progress bar
            

        // configure test functionality 
        transport_set(&fifo_io);                       // set current transport to fifo. 
 
        st_clear_log();                                  // reset all previous loged data. 
        st_logging_active();                             // set selftest logging active
        
        // record test start time 
        self_test::next_transmission_time = micros();  // set the start time of the self test

        // get first random injection position
        next_injection_position = random(0, 10);       // set the random injection possition
    }


    // create a non-blocking loop to only send packts after alocatred time. 
    if(micros() > self_test::next_transmission_time){

        // print progress bar 
        uint8_t one_percent = no_of_packets / PROGRESS_BAR_COUNT;

        if(self_test::current_test_packet < no_of_packets && self_test::current_test_packet % one_percent == 0 ){
            PRINT_PROGRESS_BAR_PROGRESS();
        }
        
        // set the next transmision time
        self_test::next_transmission_time += DEFAULT_PACKET_DELAY_US;
    
        //run the selftest 
        if(self_test::current_test_packet < no_of_packets){
            
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", "packets sent current packet count: ", self_test::current_test_packet);
            
            // bult data frame
            uint8_t type = MSG_TEST_VALID;                                        // send all packets with a valid type
            uint8_t ack  = TRANSPORT_ACK_TYPE::NORMAL_FRAME;                      // send all packets as normal frames
            uint8_t dlc  = random(1,(MAX_PAYLOAD_LEN + 1));   // set random dlc 1 - MAX_PAYLOAD
            uint8_t data[MAX_PAYLOAD_LEN]; 
            for(int i = 0; i < dlc; i++){
                data[i] = random(0,255);
            }

            //inject the error at a random interval
            if(self_test::current_test_packet % 10 == next_injection_position){
                st_log_injected_error();  // log injected errors here
                set_tx_fault_injection_active(fault_type, fault_value);
            }

            // set the next random packet every 10 packets 
            if(self_test::current_test_packet % 10 == 0){
                next_injection_position = random(0, 10);
            }

            // transmit packet
            transport_queue_message(type, ack, dlc, data);

            //increment current packet
            self_test::current_test_packet ++; 
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", " packets sent. current_test_packets: ", self_test::current_test_packet);
        }

    }

    // run the self test for a pre defined time after the final packet is sent
    if(self_test::current_test_packet == no_of_packets && self_test::test_end_countdown_timer == false){
        self_test::test_end_countdown_timer = micros();
        self_test::test_end_counter = true; 
    }


    // exit the self test runs one at the end of the test
    if(self_test::test_end_counter == true && micros() - self_test::test_end_countdown_timer > TEST_END_COUNTDOWN_TIMER_US){

        PRINT_PROGRESS_BAR_END();                                   // Progress bar

        self_test::current_test_packet = 0;                         // reset the packet count a the end of the test 

        //transport_set(&uart_io);   //*** DISABLED FOR TESTING *** // set transport back to uart on completion of test.
        //set_transport_selftest_loging_inactive();                  REMOVE 
        st_logging_inactive();                                      // dissable loging once test is completed 
        enable_verbous_error();                                     // enable verbous logging

        // check test results here
        bool test_result = true; 
        
        test_result &= st_check_test_result(ST_TEST_ENTRY::ST_LOG_TOTAL_ERRORS, EVALUATION_TYPE::EQUAL, error_count);
        test_result &= st_check_test_result(ST_TEST_ENTRY::ST_LOG_PACKETES_RECEIVED, EVALUATION_TYPE::EQUAL, (no_of_packets - error_count)); 
        test_result &= st_compare_test_result(ST_TEST_ENTRY::ST_LOG_TOTAL_ERRORS, EVALUATION_TYPE::EQUAL, ST_TEST_ENTRY::ST_LOG_INJECTED_ERROR);

        if(test_result == true){
            return SELFTEST_PASSED;
        }
        else{
            st_print_log();
            return SELFTEST_FAILED;
        } 
    }
    return SELFTEST_RUNNING;  
}
