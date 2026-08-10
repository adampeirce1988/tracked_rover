

#include <Arduino.h>
#include "self_test_internal.h"
#include "self_test.h" 
#include "debug.h"

#define DEBUG_FILE DBG_SELF_TEST

//===================== External APIs =====================

// used to request a selftest to start
bool request_self_test(TEST_ID test_id){

    // check that test manager is idle 
    if(self_test::manager_ctx.manager_state != SELF_TEST_MANAGER::IDLE){
        // print an error meassage 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "Self-test currently in progress."); 
    
        return false;
    }
    // check test selection is valid
    if(test_id == TEST_ID::NO_TEST_SELECTED){

        // print an error meassage 
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TEST", "Invalid test requested:", get_test_name(test_id)); 
        
        return false;
    } 
    
    // Set the requeted_test_id
    self_test::runtime_ctx.requested_test_id = test_id; 

    // print confirmation of test if debug enabled
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TEST", "Test requested:", get_test_name(test_id)); 

    return true; 
}

bool diagnostics_active(){
    return self_test::runtime_ctx.diagnostics_active; 
}
//=========================================================
