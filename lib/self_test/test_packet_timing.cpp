
#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "logger.h" 
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST

// test random number config
constexpr uint8_t MIN_DATA_VALUE = 0; 
constexpr uint16_t MAX_DATA_VALUE = 256;
constexpr uint8_t MIN_TYPE_VALUE = 1; 
constexpr uint16_t MAX_TYPE_VALUE = 256;


TEST_RETURN_STATUS self_test_transport_random_packet(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active){
    
    // Cache micro to prvent recalling agian in function
    uint32_t cached_micros = micros(); 

    // test setup(runs only once)
    if(self_test::runtime_ctx.current_test_packet == 0){
        
        // set current transport to fifo.
        transport_set(&fifo_io);                   
        
        self_test::runtime_ctx.next_transmission_time = cached_micros;

        // print the porgress bar & protect from 0 devision
        self_test::runtime_ctx.progress_bar_one_percent = no_of_packets / PROGRESS_BAR_COUNT; 
        if(self_test::runtime_ctx.progress_bar_one_percent == 0){ 
            self_test::runtime_ctx.progress_bar_one_percent = 1; 
        }

        // print information (only at the sart of the test)
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
        PRINT_PROGRESS_BAR_START();
    }

    // create a non-blocking loop to only send packts after alocatred time. 
    if(cached_micros > self_test::runtime_ctx.next_transmission_time){

         
        // print the progress bar if 1% has passed
        if(self_test::runtime_ctx.current_test_packet < no_of_packets && self_test::runtime_ctx.current_test_packet % self_test::runtime_ctx.progress_bar_one_percent == 0){
            PRINT_PROGRESS_BAR_PROGRESS();
        }
        
        // set the next transmision time
        self_test::runtime_ctx.next_transmission_time += delay_time_us;


        // reducetime by a randon amount if random delay is requested.
        if(random_delay_active){
            uint16_t reduction = weighted_random_delay(delay_time_us);
            self_test::runtime_ctx.next_transmission_time -= reduction; 
            st_log_delayed_packet();
        }
    
        //build random packet 
        if(self_test::runtime_ctx.current_test_packet < no_of_packets){
            
            uint8_t type = random(MIN_TYPE_VALUE, MAX_TYPE_VALUE);    // random type 1-255 ** NO TYPE CHECK IMPLIMENTED **
            uint8_t ack  = weighted_random_ack();                     // weighted ack 20% chance of ack 
            uint8_t dlc  = random(0,(MAX_PAYLOAD_LEN + 1));           // set random dlc
            uint8_t data[MAX_PAYLOAD_LEN]; 
            for(int i = 0; i < dlc; i++){
                data[i] = random(MIN_DATA_VALUE, MAX_DATA_VALUE);
            }

            // report when an  ack is sent. 
            if(ack == TRANSPORT_ACK_TYPE::ACK_REQUEST){
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "ACK", "random packet sent with ack request set. ack_request: ", ack);
            } 

            // transmit packet
            transport_queue_message(type, ack, dlc, data);

            //increment current packet
            self_test::runtime_ctx.current_test_packet ++; 
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", " packets sent current packet count: ", self_test::runtime_ctx.current_test_packet);
        }
    }  

    // run the self test for a pre defined time after the final packet is sent to clear the fifo buffer
    if(self_test::runtime_ctx.current_test_packet == no_of_packets && self_test::runtime_ctx.test_end_countdown_timer == false){
        self_test::runtime_ctx.test_end_countdown_timer = cached_micros;
        self_test::runtime_ctx.test_end_countdown_active = true; 
    }

    // exit the self test runs one at the end of the test
    if(self_test::runtime_ctx.test_end_countdown_active && cached_micros - self_test::runtime_ctx.test_end_countdown_timer > TEST_END_COUNTDOWN_TIMER_US){
    
        // Test results agianst pass critera  
        bool test_result = true; 
        test_result &= st_compare_test_result(ST_TEST_ENTRY::PACKETS_SENT, EVALUATION_TYPE::EQUAL, ST_TEST_ENTRY::PACKETS_RECEIVED); 
        test_result &= st_check_test_result(ST_TEST_ENTRY::PACKETS_RECEIVED, EVALUATION_TYPE::EQUAL, no_of_packets);
        test_result &= st_check_test_result(ST_TEST_ENTRY::PACKETS_SENT, EVALUATION_TYPE::EQUAL, no_of_packets);
        test_result &= st_compare_test_result(ST_TEST_ENTRY::ACKS_TRANSMITTED, EVALUATION_TYPE::EQUAL, ST_TEST_ENTRY::ACK_RECEIVED);

        PRINT_PROGRESS_BAR_END();
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "SELF", "result of assesment for self_test. test_result: ", test_result );

        
        if(test_result){
            return TEST_RETURN_STATUS::PASSED; 
        }
        else{
            st_print_log(); 
            return TEST_RETURN_STATUS::FAILED;  
        }   
    }
    return TEST_RETURN_STATUS::RUNNING; 
}

