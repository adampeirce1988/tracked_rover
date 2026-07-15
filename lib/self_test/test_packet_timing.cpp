
#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "logger.h" 
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST


TEST_RETURN_STATUS self_test_transport_random_packet(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active){
    
    // test setup(runs only once)
    if(self_test::runtime_ctx.current_test_packet == 0){

        transport_set(&fifo_io);                  // set current transport to fifo. 
        // **moved to test_manager ** st_clear_log();                           // rest selftest error log ** NEW FUNCTIOON **
        // **moved to test_manager ** st_enable_logging();                      // activate loging set to test exit 
        // **moved to test_manager ** disable_verbose_error();                  // disable verbous errors during the test.

        self_test::runtime_ctx.next_transmission_time = micros();

        // print information (only at the sart of the test)
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
        PRINT_PROGRESS_BAR_START();
    }

    // create a non-blocking loop to only send packts after alocatred time. 
    if(micros() > self_test::runtime_ctx.next_transmission_time){

        // print the porgress bar
        uint8_t one_percent = no_of_packets / PROGRESS_BAR_COUNT; 
        if(one_percent == 0){ // protect from 0 devision 
            one_percent = 1; 
        }
         

        if(self_test::runtime_ctx.current_test_packet < no_of_packets && self_test::runtime_ctx.current_test_packet % one_percent == 0){
            PRINT_PROGRESS_BAR_PROGRESS();
        }
        
        // set the next transmision time
        self_test::runtime_ctx.next_transmission_time += delay_time_us;


        // reducetime by a randon amount if random delay is requested.
        if(random_delay_active == true){
            uint16_t reduction = weighted_random_delay(delay_time_us);
            self_test::runtime_ctx.next_transmission_time = self_test::runtime_ctx.next_transmission_time - reduction; 
            st_log_delayed_packet();
        }
    
        //build random packet 
        if(self_test::runtime_ctx.current_test_packet < no_of_packets){
            
            uint8_t type = random(1, 256);                    // random type 1-255 ** NO TYPE CHECK IMPLIMENTED **
            uint8_t ack  = weighted_random_ack();             // weighted ack 20% chance of ack 
            uint8_t dlc  = random(0,(MAX_PAYLOAD_LEN + 1));   // set random dlc
            uint8_t data[MAX_PAYLOAD_LEN]; 
            for(int i = 0; i < dlc; i++){
                data[i] = random(0,255);
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
        self_test::runtime_ctx.test_end_countdown_timer = micros();
        self_test::runtime_ctx.test_end_countdown_ative = true; 
    }

    // exit the self test runs one at the end of the test
    if(self_test::runtime_ctx.test_end_countdown_ative == true && micros() - self_test::runtime_ctx.test_end_countdown_timer > TEST_END_COUNTDOWN_TIMER_US){
    
        // Test results agianst pass critera  
        bool test_result = true; 
        test_result &= st_compare_test_result(ST_TEST_ENTRY::PACKETS_SENT, EVALUATION_TYPE::EQUAL, ST_TEST_ENTRY::PACKETS_RECEIVED); 
        test_result &= st_check_test_result(ST_TEST_ENTRY::PACKETS_RECEIVED, EVALUATION_TYPE::EQUAL, no_of_packets);
        test_result &= st_check_test_result(ST_TEST_ENTRY::PACKETS_SENT, EVALUATION_TYPE::EQUAL, no_of_packets);
        test_result &= st_compare_test_result(ST_TEST_ENTRY::ACKS_TRANSMITTED, EVALUATION_TYPE::EQUAL, ST_TEST_ENTRY::ACK_RECEIVED);

        PRINT_PROGRESS_BAR_END();
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "SELF", "result of assesment for self_test. test_result: ", test_result );

        // **moved to test_manager ** transport_set(&uart_io); //*** DISABLED FOR TESTING ***  // set transport back to uart on completion of test.
    
        // **moved to test_manager ** st_disable_logging();                                     // dissable loging once test is completed 
        // **moved to test_manager ** enable_verbose_error();                                    // enable verbous error 

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

