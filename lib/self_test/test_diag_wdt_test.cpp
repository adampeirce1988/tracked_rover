#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "logger.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST

/*
 * Verifies that the Diagnostic watchdog timeout is detected correctly.
 *
 * The manager intentionally allows the watchdog to expire during this
 * test and then resumes execution once the timeout event has been
 * recorded. The test passes when the watchdog event is successfully
 * logged before the timeout window expires.
 */ 


TEST_RETURN_STATUS self_test_diagnostics_wdt(){

    // Cache the current time for this iteration.
    const uint32_t cached_millis = millis(); 

    // One-time test initialisation.
    if(!self_test::runtime_ctx.watchdog_timer_test_active){
        
        self_test::runtime_ctx.progress_bar_one_percent = DIAGNOSTIC_WT_TIMEOUT_MS / PROGRESS_BAR_COUNT;
        
        // Add 1 ms to the test end counter to cause the watchdog timer to triger
        self_test::runtime_ctx.test_end_countdown_timer = cached_millis + (DIAGNOSTIC_WT_TIMEOUT_MS + 1); 
        self_test::runtime_ctx.watchdog_timer_test_active = true; 

        // print the start of the progress bar
        PRINT_PROGRESS_BAR_START(); 
    }


    // print a progress bar to show progress i
    if(self_test::runtime_ctx.test_end_countdown_timer - cached_millis < (DIAGNOSTIC_WT_TIMEOUT_MS - (self_test::runtime_ctx.progress_bar_one_percent * self_test::runtime_ctx.progress_bar_position))){
        PRINT_PROGRESS_BAR_PROGRESS();
        self_test::runtime_ctx.progress_bar_position ++; 

    }


    if(st_check_test_result(ST_TEST_ENTRY::WDT_TIMEOUT,EVALUATION_TYPE::EQUAL ,1)){
        
        // print the end of the progress bar & rest the counter. 
        PRINT_PROGRESS_BAR_END(); 
        
        // set the test status to false
        self_test::runtime_ctx.watchdog_timer_test_active = false; 
        
        return TEST_RETURN_STATUS::PASSED;
    }
    else if(self_test::runtime_ctx.watchdog_timer_test_active && cached_millis > self_test::runtime_ctx.test_end_countdown_timer){
        
        // print the end of the progress bar & rest the counter. 
        PRINT_PROGRESS_BAR_END(); 

        // set the test status to false
        self_test::runtime_ctx.watchdog_timer_test_active = false; 

        return TEST_RETURN_STATUS::FAILED;
    }

    return TEST_RETURN_STATUS::RUNNING;
}