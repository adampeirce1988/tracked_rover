
#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "log.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST


uint8_t self_test_1(uint8_t current_packet, uint8_t no_of_packets, uint16_t delay_time_us, bool random_delay_active){
    
    // test setup(runs only once)
    if(current_packet == 0){

        transport_set(&fifo_io);                  // set current transport to fifo. 
        transport_selftest_log_clear();           // reset all previous loged data.  *** MAY NEED TAYLORING TO SPECIFIC TEST TYPES **
        set_transport_selftest_loging_active();   // activate loging

        self_test::next_transmission_time = micros();
        // print information (only at the sart of the test)
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "running self_test_1 good packet test: ", no_of_packets, " msgs.");
        DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "self_test_1 tests packets transmitted speed: ", delay_time_us, " us");
    }

    // create a non-blocking loop to only send packts after alocatred time. 
    if(micros() > self_test::next_transmission_time){
        
        // set the next transmision time
        self_test::next_transmission_time += delay_time_us;
        // reduce time by a randon amount if random delay is requested.

        if(random_delay_active == true){
            //self_test::next_transmission_time -= weighted_random_dealy(delay_time_us); 
        }
    
        //build random packet 
        if(current_packet < (no_of_packets)){
            
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", "packets sent current packet count: ", current_packet);

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
        }
            
        // runs one at the end of the test
        if(current_packet == (no_of_packets)){ 
            // reset current packet
            self_test::current_test_packet = 0; 

            // change this to retun a result if no errors are loged if errors are loged then return an error reprot.
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TEST", "self_test_1 completed result: UNKNOWN"); // change to return

            
            //transport_set(&uart_io); *** DISABLED FOR TESTING *** // set transport back to uart on completion of test.
            set_transport_selftest_loging_inactive();               // dissable loging once test is completed 
            

            // check for pass or fail 0 errors expected. 
            if(transport_test_log.total_errors == 0 && transport_test_log.packets_received == no_of_packets){ // change to a function call that will accept ans expectd test result array. 
                return SELFTEST_PASSED; // assess test results and mark pass/ fail flag 
            }
            else{
                return SELFTEST_FAILED;
            }
            
        }
       
    } 
    return SELFTEST_RUNNING; 
}

