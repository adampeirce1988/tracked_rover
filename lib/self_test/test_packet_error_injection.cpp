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

// random number config
constexpr uint8_t MIN_INJECTION_POSITION = 0; 
constexpr uint8_t MAX_INJECTION_POSITION = 10;
constexpr uint8_t MIN_DATA_VALUE = 1; 
constexpr uint8_t MAX_DATA_VALUE = 255;


TEST_RETURN_STATUS self_test_error_injection(uint8_t no_of_packets, uint8_t error_count, TX_SET_FAULT_MODE fault_type, uint8_t fault_value){

    // Cache micro to prvent recalling agian in function
    uint32_t cached_micros = micros(); 

    // test setup(runs only once)
    if(self_test::runtime_ctx.current_test_packet == 0){

        //print test inoformartion // *** TODO: MAKE TESTING TYOE DYNAMIC AND REFLECT SELECTED TEST TYPE ***
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "running self_test_2 corrup crc error count: ", error_count, " corupt frames.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_1 tests packets transmitted speed: ", DEFAULT_PACKET_DELAY_US, " us"); 
    
        // configure progress bar 1% and protect against 0 devision
        self_test::runtime_ctx.progress_bar_one_percent = no_of_packets / PROGRESS_BAR_COUNT;
        if(self_test::runtime_ctx.progress_bar_one_percent == 0){
            self_test::runtime_ctx.progress_bar_one_percent = 1; 
        }

        // print the start of the progress bar
        PRINT_PROGRESS_BAR_START();             
            
        // set current transport to fifo. 
        transport_set(&fifo_io);                       
        
        // record test start time 
        self_test::runtime_ctx.next_transmission_time = cached_micros;  // set the start time of the self test

        // get first random injection position
        self_test::runtime_ctx.next_injection_position = random(MIN_INJECTION_POSITION, MAX_INJECTION_POSITION);       
    }


    // create a non-blocking loop to only send packts after alocatred time. 
    if(cached_micros > self_test::runtime_ctx.next_transmission_time){


        // print the progress bar counter if 1% has passed
        if(self_test::runtime_ctx.current_test_packet < no_of_packets && self_test::runtime_ctx.current_test_packet % self_test::runtime_ctx.progress_bar_one_percent == 0 ){
            PRINT_PROGRESS_BAR_PROGRESS();
        }
        
        // set the next transmision time
        self_test::runtime_ctx.next_transmission_time += DEFAULT_PACKET_DELAY_US;
    
        //run the selftest 
        if(self_test::runtime_ctx.current_test_packet < no_of_packets){
            
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", "packets sent current packet count: ", self_test::runtime_ctx.current_test_packet);
            
            // bult data frame
            uint8_t type = MSG_TEST_VALID;                                        // send all packets with a valid type
            uint8_t ack  = TRANSPORT_ACK_TYPE::NORMAL_FRAME;                      // send all packets as normal frames
            uint8_t dlc  = random(1,(MAX_PAYLOAD_LEN + 1));   // set random dlc 1 - MAX_PAYLOAD
            uint8_t data[MAX_PAYLOAD_LEN]; 
            for(int i = 0; i < dlc; i++){
                data[i] = random(MIN_DATA_VALUE, MAX_DATA_VALUE);
            }

            //inject the error at a random interval
            if(self_test::runtime_ctx.current_test_packet % 10 == self_test::runtime_ctx.next_injection_position){
                st_log_injected_error();  // log injected errors here
                set_tx_fault_injection_active(fault_type, fault_value);
            }

            // set the next random packet every 10 packets 
            if(self_test::runtime_ctx.current_test_packet % 10 == 0){
                self_test::runtime_ctx.next_injection_position = random(MIN_INJECTION_POSITION, MAX_INJECTION_POSITION);
            }

            // transmit packet
            transport_queue_message(type, ack, dlc, data);

            //increment current packet
            self_test::runtime_ctx.current_test_packet ++; 
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", " packets sent. current_test_packets: ", self_test::runtime_ctx.current_test_packet);
        }

    }

    // run the self test for a pre defined time after the final packet is sent
    if(self_test::runtime_ctx.current_test_packet == no_of_packets && self_test::runtime_ctx.test_end_countdown_timer == false){
        self_test::runtime_ctx.test_end_countdown_timer = cached_micros;
        self_test::runtime_ctx.test_end_countdown_active = true; 
    }


    // exit the self test runs one at the end of the test
    if(self_test::runtime_ctx.test_end_countdown_active == true && cached_micros - self_test::runtime_ctx.test_end_countdown_timer > TEST_END_COUNTDOWN_TIMER_US){

        //print the end of the progress bar
        PRINT_PROGRESS_BAR_END();                                   

        // reset the packet count a the end of the test
        self_test::runtime_ctx.current_test_packet = 0;                          

        // check test results return the result. 
        bool test_result = true; 
        
        test_result &= st_check_test_result(ST_TEST_ENTRY::TOTAL_ERRORS, EVALUATION_TYPE::EQUAL, error_count);
        test_result &= st_check_test_result(ST_TEST_ENTRY::PACKETS_RECEIVED, EVALUATION_TYPE::EQUAL, (no_of_packets - error_count)); 
        test_result &= st_compare_test_result(ST_TEST_ENTRY::TOTAL_ERRORS, EVALUATION_TYPE::EQUAL, ST_TEST_ENTRY::INJECTED_ERROR);

        if(test_result == true){
            return TEST_RETURN_STATUS::PASSED;
        }
        else{
            st_print_log();
            return TEST_RETURN_STATUS::FAILED;
        } 
    }
    return TEST_RETURN_STATUS::RUNNING;  
}
