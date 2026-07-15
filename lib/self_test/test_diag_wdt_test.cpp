#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "logger.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST

// this test will check that the timeout function of transport is correctly functioning this will not report a pass or fall 
// onlly indication of a sucsesfullt test is the main entering SAFE_STATR and ERROR being displayed. 

// progress bar variables 
constexpr uint16_t divisor = DIAGNOSTIC_WT_TIMEOUT_MS / 25; 
uint8_t print_counter = 1;

TEST_RETURN_STATUS self_test_diagnostics_wdt(){

    // runs once on first loop
    if(self_test::runtime_ctx.watchdog_timer_test_active == false){
        disable_verbose_error();                                // dissable verbous error reporting
        st_enable_logging();                                     // activate st_logging

        self_test::runtime_ctx.test_end_countdown_timer = millis() + (DIAGNOSTIC_WT_TIMEOUT_MS + 1);  
        self_test::runtime_ctx.watchdog_timer_test_active = true; 

        PRINT_PROGRESS_BAR_START(); 
    }


    // print a progress bar to show progress i
    if(self_test::runtime_ctx.test_end_countdown_timer - millis() < (DIAGNOSTIC_WT_TIMEOUT_MS - (divisor * print_counter))){
        PRINT_PROGRESS_BAR_PROGRESS();
        print_counter ++;

    }


    if(st_check_test_result(ST_TEST_ENTRY::WDT_TIMEOUT,EVALUATION_TYPE::EQUAL ,1)){
        PRINT_PROGRESS_BAR_END(); 
        self_test::runtime_ctx.watchdog_timer_test_active = false; 
        print_counter = 1; 
        enable_verbose_error(); // enable verbous error reporting
        st_disable_logging();  // disable st_logging

        return TEST_RETURN_STATUS::PASSED;
    }
    else if(self_test::runtime_ctx.watchdog_timer_test_active == true && millis() > self_test::runtime_ctx.test_end_countdown_timer){
        PRINT_PROGRESS_BAR_END(); 
        st_print_log();
        self_test::runtime_ctx.watchdog_timer_test_active = false; 
        print_counter = 1; 
        enable_verbose_error(); // enable verbous error reporting
        st_disable_logging();  // disable st_logging

        return TEST_RETURN_STATUS::FAILED;
    }

    return TEST_RETURN_STATUS::RUNNING;
}