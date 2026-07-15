
#include<stdint.h>
#include <Arduino.h>
#include "self_test_internal.h"
#include "global_config.h"
#include "self_test.h"
#include "transport.h"
#include "logger.h"
#include "debug.h"

#define DEBUG_FILE DBG_SELF_TEST // defines the debug logging file (defined in debug_config.h)

// change over fromt the name space to namespace & struct 
namespace self_test{
    SelfTestManagerContext manager_ctx;
    SelfTestRuntimeContect runtime_ctx;
}

/*-------------------------------------------------------
Self Test Manager

This FSM owns the lifecycle of every self test.

Responsibilities:
    - Accept test requests
    - Initialise the environment
    - Execute the selected test
    - Monitor watchdog timeout
    - Handle abort requests
    - Record final status
    - Restore the system

Individual test implementations never manipulate
manager_state directly. They return a TEST_RETURN_STATUS
which the manager interprets.

-------------------------------------------------------*/


SELF_TEST_MANAGER_RETURN_CODE run_test_manager(){

    switch(self_test::manager_ctx.manager_state){

        case SELF_TEST_MANAGER::IDLE:{
            // listen for a test call & advance when received 
            if(self_test::runtime_ctx.requested_test_id != TEST_ID::NO_TEST_SELECTED){

                self_test::runtime_ctx.current_active_test_id = self_test::runtime_ctx.requested_test_id; 

                self_test::runtime_ctx.requested_test_id = TEST_ID::NO_TEST_SELECTED;

                self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::STARTING; 

                self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_STARTING;
            }
            else{
                self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::NO_TEST_RUNNING;
            }
        } break;

        case SELF_TEST_MANAGER::STARTING:{

            st_clear_log();            // clear logs
            disable_verbose_error();   // disable verbose error reporting             
            st_enable_logging();       // enable self_test logging

            self_test::runtime_ctx.watchdog_timestamp = millis();  
            
            self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::RUNNING;

            self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_STARTING;
        } break;

        case SELF_TEST_MANAGER::RUNNING:{

            // run the test case here untill pass/ fail/ abort/ timeout. 
            self_test::runtime_ctx.current_test_status_code = run_test_case(); 

            if(self_test::runtime_ctx.current_test_status_code != TEST_RETURN_STATUS::RUNNING){ // check this never returns IDLE. 
                self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::FINALIZING; 
            }

           self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_RUNNING;
        } break;

        
        case SELF_TEST_MANAGER::FINALIZING:{
            // Restore normal logging/debug behaviour and process results

            enable_verbose_error();   // return to verbose output over the serial port
            st_disable_logging();     // disssable self test logging 

            switch (self_test::runtime_ctx.current_test_status_code){

                case TEST_RETURN_STATUS::PASSED:
                    self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::PASSED; 
                break; 

                case TEST_RETURN_STATUS::FAILED:
                    self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::FAILED; 
                break; 

                default: 
                    // report an error and run clean up
                    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_", get_test_name(self_test::runtime_ctx.current_active_test_id), "returned undefined return code");
                    self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::CLEANUP;
                break;
            }
   
            self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_FINALIZING; 

        } break;
 
        case SELF_TEST_MANAGER::PASSED:{

            // handle pass information (send result to web interface)
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_", get_test_name(self_test::runtime_ctx.current_active_test_id), ": PASSED.");

            self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::CLEANUP;

            self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_PASSED;
        } break;

        case SELF_TEST_MANAGER::FAILED:{

            // print results
            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_META, "TEST", "self_test_", get_test_name(self_test::runtime_ctx.current_active_test_id), ": FAILED.");

            // print result. 
            st_print_log(); 

            self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::CLEANUP;

            self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_FAILED;
        } break;

        case SELF_TEST_MANAGER::TIMEOUT:{

            // The watchdog timeout test intentionally expires the manager watchdog.
            // Log the timeout, extend the watchdog, and resume execution so the
            // test case can verify that the watchdog event was correctly recorded.         
            if(self_test::runtime_ctx.current_active_test_id == TEST_ID::DIAGNOSTIC_WATCHDOG_TIMEOUT && !self_test::runtime_ctx.watchdog_event_verified){

                st_log_wdt_triggered(); // log triggered event 

                self_test::runtime_ctx.watchdog_timestamp += WDT_EXTENSION; 
                self_test::runtime_ctx.watchdog_event_verified = true; 

                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WDT", "event_wdt_trigger activated.");

                self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::RUNNING; 
                self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_RUNNING;
            }
            else{

                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WDT", "Self test watch dog timer timed out");

                self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::CLEANUP; 
                self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_TIMEOUT;
                
            }
        } break;

        case SELF_TEST_MANAGER::ABORTED:{ // this will not yet be implemented untill the interface is working 

            DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_", get_test_name(self_test::runtime_ctx.current_active_test_id), ": ABORTED by the user.");

            self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::CLEANUP;

            self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::TEST_ABORTED;
        } break;


        case SELF_TEST_MANAGER::CLEANUP:{

            // reset all self_test variables here 
            clear_self_test_runtime_context();

            // return to uart comunication 
            transport_set(&uart_io);  // return to uart communication (Disable for testing)
            
            // return the self_test manager to idle 
            self_test::manager_ctx.manager_state = SELF_TEST_MANAGER::IDLE; 

            //return test completed for all cases after clean up. 
            return SELF_TEST_MANAGER_RETURN_CODE::TEST_COMPLETE;
        } break;


        default: {
            // report an error here
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self_test_manager entered an undefined state");  
            self_test::manager_ctx.manager_return_code = SELF_TEST_MANAGER_RETURN_CODE::ERROR;           
        } break; 

    }
    
    // monitor runtime exceptions  
    manager_runtime_monitor();

    return self_test::manager_ctx.manager_return_code; 
}