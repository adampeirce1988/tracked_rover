

#include<stdint.h>
#include "self_test_internal.h"
#include "self_test.h"
#include "debug.h"

#define DEBUG_FILE DBG_SELF_TEST

TEST_RETURN_STATUS execute_self_test(){ 

    switch(self_test::runtime_ctx.current_active_test_id){
        
        case TEST_ID::NO_TEST_SELECTED:{
            // do nothing until a test case is selected via request test casee 
            return TEST_RETURN_STATUS::IDLE;
        }

        // Transport good packet test with moderate timing 
        case TEST_ID::TRANSPORT_PACKET_NORMAL:{

            return self_test_transport_random_packet(
                STANDARD_TEST_PACKET_COUNT, 
                STANDARD_PACKET_DELAY_US, 
                DISABLE_RANDOM_PACKET_TIMING_DELAY
            ); 
        }
       
        
         //Transport good packet test with tight timing and random reduced time between transmisios (1:10)
        case TEST_ID::TRANSPORT_PACKET_STRESS:{ 

            return self_test_transport_random_packet(
                STRESS_TEST_PACKET_COUNT, 
                STRESS_TEST_PACKET_DELAY_US, 
                ENABLE_RANDOM_PACKET_TIMING_DELAY
            ); 
        }
        

        // Transport change the type. 
        // Result: CRC errors
        case TEST_ID::TRANSPORT_TYPE_CHANGE:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::TYPE_CHANGE
            );
        } 

        // Transport change the ACK
        // Result: CRC errors
        case TEST_ID::TRANSPORT_ACK_CHANGE:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::ACK_CHANGE, 
                TRANSPORT_ACK_TYPE::NACK
            );
        } 

        // Transport change the ID
        // Result: CRC errors
        case TEST_ID::TRANSPORT_ID_CHANGE:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::ID_CHANGE
            );
        }
        

        // Transport change the DLC
        // Result: CRC errors
        case TEST_ID::TRANSPORT_DLC_CHANGE:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DLC_CHANGE
            );
        }
        
        // Transport change DLC length
        // Result: Overflow error
        case TEST_ID::TRANSPORT_DLC_OVERFLOW:{
            
            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::DLC_OVER_MAX_CAPACITY
            );
        }


        // Transport 1 bit in the CRC fliped randomly changed on 1 packet in 10
        // Result: CRC errors
        case TEST_ID::TRANSPORT_CRC_BIT_FLIP:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::CRC_RAND_FLIP_BIT
            );
        }
       
        // Transport change the CRC
        // Result: CRC errors
        case TEST_ID::TRANSPORT_CRC_RANDOM_CHANGE:{
            
            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT, 
                INJECTION_TEST_ERROR_COUNT, 
                TX_SET_FAULT_MODE::CRC_CHANGE,
                CRC_RANDOM_REPLACEMENT_VALUE
            );
        } 
    
        // Transport flip data bit.
        // Result: CRC errors
        case TEST_ID::TRANSPORT_DATA_BIT_FLIP:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DATA_FLIP_BIT
            );
        }

        // Transport change data byte
        // Result: CRC errors
        case TEST_ID::TRANSPORT_DATA_BYTE_CHANGE:{

            return self_test_error_injection(
                INJECTION_TEST_PACKET_COUNT,
                INJECTION_TEST_ERROR_COUNT,
                TX_SET_FAULT_MODE::RAND_DATA_CHANGE
            );
        }
    

        // Diagnostic Watchdog timer error
        // Result: watchdog timer trigered
        case TEST_ID::DIAGNOSTIC_WATCHDOG_TIMEOUT:{
            return self_test_diagnostics_wdt();
        }

        default:{
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "Unknown self test ID.");
            return TEST_RETURN_STATUS::ERROR;
        }
    }
}



