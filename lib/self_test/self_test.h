#ifndef SELF_TEST_H 
#define SELF_TEST_H

#include <stdbool.h>
#include "self_test_types.h"

//====================== API function calls  ======================

// Runs one iteration of the self-test manager.
// Must be called periodically from the main loop.
SELF_TEST_MANAGER_RETURN_CODE run_test_manager();

// Requests a self-test.
// Returns false if the request is rejected.
bool request_self_test(TEST_ID test_id);

// Requests the active self-test to abort.
// Cleanup is performed by the self-test manager.
void abort_current_test();            

//===============================================================

#endif
