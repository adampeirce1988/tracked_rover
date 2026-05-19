#include <Arduino.h>
#include "global_config.h"
#include "transport.h"
#include "log.h"
#include "self_test.h"
#include "debug.h"
#include "debug_config.h"
#include "self_test_internal.h"

#define DEBUG_FILE DBG_SELF_TEST


// this test will check that the timeout function of transport is correctly functioning this will not report a pass or fall 
// onlly indication of a sucsesfullt test is the main entering SAFE_STATR and ERROR being displayed. 

// test variabls  
uint8_t print_counter = 1;

uint8_t self_test_6(){

        // print a progress bar
    uint16_t divisor = DIAGNOSTIC_WT_TIMEOUT_MS / 25;


    // runs once on first loop
    if(self_test::watchdog_timer_test_active == false){
        self_test::test_end_countdown_timer = millis() + (DIAGNOSTIC_WT_TIMEOUT_MS + 1);  
        self_test::watchdog_timer_test_active = true; 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_NONE, "TEST", "diagnostics watdogtimer test in progress:"); 
        PRINT_PROGRESS_BAR_START(); 

    }

    // print a progress bar to show progress i
    if(self_test::test_end_countdown_timer - millis() < (DIAGNOSTIC_WT_TIMEOUT_MS - (divisor * print_counter))){
        PRINT_PROGRESS_BAR_PROGRESS();
        print_counter ++;
    }


    if(transport_test_log.diagnostics_wdt_test_result == true){
        PRINT_PROGRESS_BAR_END(); 
        self_test::watchdog_timer_test_active = false; 
        return SELFTEST_PASSED;
    }
    else if(self_test::watchdog_timer_test_active == true && millis() > self_test::test_end_countdown_timer){
        PRINT_PROGRESS_BAR_END(); 
        self_test::watchdog_timer_test_active = false; 
        print_counter = 1; 
        return SELFTEST_FAILED;
    }
    else{
        return SELFTEST_RUNNING;
    }
}