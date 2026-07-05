

#include <Arduino.h>
#include "self_test_internal.h"
#include "global_config.h"
#include "transport.h"
#include "self_test.h" 
#include "debug.h"

#define DEBUG_FILE DBG_SELF_TEST

//===================== External APIs =====================

// used to request a selftest to start
bool request_self_test(TEST_ID test_id){
    if(check_diagnostic_status() == true){
        // report that a test is in pogress. 
        // also handle in the web ui by block test selection once a test is stated
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self-test currntly in progres."); 
    }
    else{
        test_request_dispatcher(test_id);
    }

    return true; 
}

// returns ture if a test is in progess
bool check_diagnostic_status(){
    return self_test::ctx.diagnostics_active; 
}

// returns true if a watchdog timer test is active 
bool watchdog_test_active(){
    return self_test::ctx.watchdog_timer_test_active;
}
//=========================================================
