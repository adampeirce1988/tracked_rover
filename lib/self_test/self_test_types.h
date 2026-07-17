#ifndef SELF_TEST_TYPES_H
#define SELF_TEST_TYPES_H

#include <stdint.h>

//======================================================
// Available Self Tests
// Identifies which test the user/system wants to run
//======================================================

enum class TEST_ID : uint8_t
{
    NO_TEST_SELECTED = 0,

    // Transport basic tests
    TRANSPORT_PACKET_NORMAL,          // Run a valid packet test with moderate timing
    TRANSPORT_PACKET_STRESS,          // Run a valid packet test with reduced/random timing

    // Transport header fault injection
    TRANSPORT_TYPE_CHANGE,            // Modify TYPE field, expect CRC failure
    TRANSPORT_ACK_CHANGE,             // Modify ACK field, expect CRC failure
    TRANSPORT_ID_CHANGE,              // Modify packet ID, expect CRC failure

    // Transport DLC tests
    TRANSPORT_DLC_CHANGE,             // Change DLC to a random valid value
    TRANSPORT_DLC_OVERFLOW,           // Test DLC exceeding maximum payload size

    // CRC fault injection
    TRANSPORT_CRC_BIT_FLIP,           // Flip a CRC bit, expect CRC failure
    TRANSPORT_CRC_RANDOM_CHANGE,      // Replace CRC randomly, expect CRC failure

    // Payload fault injection
    TRANSPORT_DATA_BIT_FLIP,          // Flip random data bit, expect CRC failure
    TRANSPORT_DATA_BYTE_CHANGE,       // Change random data byte, expect CRC failure

    // Diagnostics
    DIAGNOSTIC_WATCHDOG_TIMEOUT       // Verify watchdog timeout handling
};


//======================================================
// Self Test Manager State Machine
// Internal FSM control states
//======================================================

enum class SELF_TEST_MANAGER : uint8_t
{
    IDLE,          // Waiting for a test request

    STARTING,      // Initialising selected test
    RUNNING,       // Executing selected test
    FINALIZING,    // finalize the test enable debugging 

    PASSED,        // Test completed successfully
    FAILED,        // Test failed
    TIMEOUT,       // Test exceeded allowed runtime
    ABORTED,       // Test stopped externally

    CLEANUP        // Clear resources and return to idle
};


//======================================================
// Individual Test Return Status
// Returned by run_test_case()
//======================================================

enum class TEST_RETURN_STATUS : uint8_t
{
    IDLE,          // No test active
    RUNNING,       // Test still executing
    PASSED,        // Test completed successfully
    FAILED,        // Test completed but with errors
    ERROR         // default return type
};


//======================================================
// Self Test Manager Return Code
// Returned by run_test_manager()
//======================================================

enum class SELF_TEST_MANAGER_RETURN_CODE : uint8_t
{
    NO_TEST_RUNNING,
    TEST_STARTING,
    TEST_RUNNING,
    TEST_FINALIZING,
    TEST_COMPLETE,
    TEST_PASSED,
    TEST_FAILED, 
    TEST_TIMEOUT,
    TEST_ABORTED,
    
    ERROR
};

#endif