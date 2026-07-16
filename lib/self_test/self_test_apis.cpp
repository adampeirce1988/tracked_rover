

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
    if(self_test::runtime_ctx.current_active_test_id != TEST_ID::NO_TEST_SELECTED){
        // print an error meassage (this may not work if a test is progress. )
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TEST", "self-test currently in progres."); 
    
        return false;
    }
    else{
        // Set the requeted_test_id
        self_test::runtime_ctx.requested_test_id = test_id; 

        return true; 
    }
        
}
//=========================================================
