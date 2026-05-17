
#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "log.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST


uint8_t self_test_1(uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active){
    
    // test setup(runs only once)
    if(self_test::current_test_packet == 0){

        transport_set(&fifo_io);                  // set current transport to fifo. 
        transport_selftest_log_clear();           // reset all previous loged data.  *** MAY NEED TAYLORING TO SPECIFIC TEST TYPES **
        set_transport_selftest_loging_active();   // activate loging
        //self_test::current_test_packet = 1;             // set the packet counter to 1 so reporting is correct

        self_test::next_transmission_time = micros();
        // print information (only at the sart of the test)
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
    }

    // create a non-blocking loop to only send packts after alocatred time. 
    if(micros() > self_test::next_transmission_time){
        
        // set the next transmision time
        self_test::next_transmission_time += delay_time_us;

        // reducetime by a randon amount if random delay is requested.
        if(random_delay_active == true){
            uint16_t reduction = weighted_random_delay(delay_time_us);
            self_test::next_transmission_time = self_test::next_transmission_time - reduction; 
        }
    
        //build random packet 
        if(self_test::current_test_packet < no_of_packets){
            
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", "packets sent current packet count: ", self_test::current_test_packet);

            uint8_t type = random(1, 256);                    // random type 1-255 ** NO TYPE CHECK IMPLIMENTED **
            uint8_t ack  = weighted_random_ack();             // weighted ack 20% chance of ack 
            uint8_t dlc  = random(0,(MAX_PAYLOAD_LEN + 1));   // set random dlc
            uint8_t data[MAX_PAYLOAD_LEN]; 
            for(int i = 0; i < dlc; i++){
                data[i] = random(0,255);
            }

            // report when an  ack is sent. 
            if(ack == ACK_REQUEST){
                DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "ACK", "random packet sent with ack request set. ack_request: ", ack);
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
        self_test::current_test_packet = 0;                     // reset the packet count a the end of the test 
        //transport_set(&uart_io); //*** DISABLED FOR TESTING *** // set transport back to uart on completion of test.
        set_transport_selftest_loging_inactive();               // dissable loging once test is completed 
        PRINT_TRANSPORT_SELFTEST_LOG();
        

        // check for pass or fail 0 errors expected. 
        if(transport_test_log.total_errors == 0 &&
           transport_test_log.packets_received == no_of_packets && 
           transport_test_log.ack_sent == transport_test_log.ack_received){
            return SELFTEST_PASSED;
        }
        else{
            return SELFTEST_FAILED;
        }   
    }
    return SELFTEST_RUNNING; 
}

