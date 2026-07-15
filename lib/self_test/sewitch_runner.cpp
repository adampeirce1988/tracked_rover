

#include<stdint.h>
#include "self_test_internal.h"
#include "self_test.h"

TEST_RETURN_STATUS run_test_case(){ 

    switch(self_test::runtime_ctx.current_active_test_id){
        
        case TEST_ID::NO_TEST_SELECTED:{
            // do nothing until a test case is selected via request test casee 
            self_test::runtime_ctx.current_test_status_code = TEST_RETURN_STATUS::IDLE;
            return self_test::runtime_ctx.current_test_status_code;
        }

        /////////////////////////////// TRANSPORT TEST 1 ///////////////////////////////
        // Run a good packet test with moderate timing 
        case TEST_ID::TRANSPORT_PACKET_NORMAL:{

            self_test::runtime_ctx.current_test_status_code = self_test_transport_random_packet(
                STANDARD_TEST_PACKET_COUNT, 
                STANDARD_PACKET_DELAY_US, 
                DISABLE_RANDOM_PACKET_TIMING_DELAY
            ); 

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);  // remove once tested
            return self_test::runtime_ctx.current_test_status_code;
        }
       
        /////////////////////////////// TRANSPORT TEST 2 ///////////////////////////////
         // run a good packet test with tight timing and random reduced time between transmisios (1:10)
        case TEST_ID::TRANSPORT_PACKET_STRESS:{ 

            self_test::runtime_ctx.current_test_status_code = self_test_transport_random_packet(
                STRESS_TEST_PACKET_COUNT, 
                STRESS_TEST_PACKET_DELAY_US, 
                ENABLE_RANDOM_PACKET_TIMING_DELAY
            ); 

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested
            return self_test::runtime_ctx.current_test_status_code;
        }
        
        /////////////////////////////// TRANSPORT TEST 3 ///////////////////////////////
        // change the type wil rsult in a crc error
        case TEST_ID::TRANSPORT_TYPE_CHANGE:{

            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::TYPE_CHANGE
            );

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code); //remove once tested

            return self_test::runtime_ctx.current_test_status_code; 
        } 

        /////////////////////////////// TRANSPORT TEST 4 ///////////////////////////////
        // change ack the will result in a crc error
        case TEST_ID::TRANSPORT_ACK_CHANGE:{

            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::ACK_CHANGE, 
                TRANSPORT_ACK_TYPE::NACK
            );
                
            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested

            return self_test::runtime_ctx.current_test_status_code; 
        } 

        /////////////////////////////// TRANSPORT TEST 5 ///////////////////////////////
        case TEST_ID::TRANSPORT_ID_CHANGE:{
        // change the current packet id will result in a crc error
            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::ID_CHANGE
            );

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested

            return self_test::runtime_ctx.current_test_status_code; 
        }
        

        /////////////////////////////// TRANSPORT TEST 6 ///////////////////////////////
        // changes the DLC to a renadom valid uint8_t
        case TEST_ID::TRANSPORT_DLC_CHANGE:{

            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DLC_CHANGE
            );

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);    // remove once tested

            return self_test::runtime_ctx.current_test_status_code; 
        }
        
        /////////////////////////////// TRANSPORT TEST 7 ///////////////////////////////
        // DLC over max capactity test expeced early catch 
        case TEST_ID::TRANSPORT_DLC_OVERFLOW:{
            
            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::DLC_OVER_MAX_CAPACITY
            );

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested
            return self_test::runtime_ctx.current_test_status_code; 
        }

        /////////////////////////////// TRANSPORT TEST 8 ///////////////////////////////
        // 1 bit in the CRC fliped randomly changed on 1 packet in 10.
        case TEST_ID::TRANSPORT_CRC_BIT_FLIP:{

            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::CRC_RAND_FLIP_BIT
            );

        // check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested
            return self_test::runtime_ctx.current_test_status_code; 
        }
       
        /////////////////////////////// TRANSPORT TEST 9 ///////////////////////////////
        // good packet test with the CRC randomly changed on 1 packet in 10. 
        case TEST_ID::TRANSPORT_CRC_RANDOM_CHANGE:{
            
            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT, 
                INJECTION_TEST_ERROR_COUNT, 
                TX_SET_FAULT_MODE::CRC_CHANGE,
                CRC_RANDOM_REPLACEMENT_VALUE
            );

            // check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested
            return self_test::runtime_ctx.current_test_status_code; 
        } 
    
        /////////////////////////////// TRANSPORT TEST 10 ///////////////////////////////
        //random data bit fliped expected crc error
        case TEST_ID::TRANSPORT_DATA_BIT_FLIP:{

            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DATA_FLIP_BIT
            );

            //check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested

            return self_test::runtime_ctx.current_test_status_code;
        }

        /////////////////////////////// TRANSPORT TEST 11 ///////////////////////////////
        //random data byte change expected crc error
        case TEST_ID::TRANSPORT_DATA_BYTE_CHANGE:{

            self_test::runtime_ctx.current_test_status_code = self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DATA_CHANGE
            );

            // check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested

            return self_test::runtime_ctx.current_test_status_code; 

        }
    

        /////////////////////////////// TEST 6 ///////////////////////////////
        case TEST_ID::DIAGNOSTIC_WATCHDOG_TIMEOUT:{
            self_test::runtime_ctx.current_test_status_code = self_test_diagnostics_wdt();
            
            // check_self_test_resutls(self_test::runtime_ctx.current_test_status_code);   // remove once tested

            return self_test::runtime_ctx.current_test_status_code; 
        }

    }
    
    return self_test::runtime_ctx.current_test_status_code; 
}