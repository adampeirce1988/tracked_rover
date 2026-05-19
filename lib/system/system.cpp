///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>

#include "system.h"
#include "protocol.h"
#include "global.h"      // TODO: Move sys:: variables into system module
#include "debug.h"
#include "self_test.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////

#define DEBUG_FILE DBG_SYSTEM


///////////////////////////////////////////////////////////////////////////////
// System Status Variables
///////////////////////////////////////////////////////////////////////////////

// Overall system health state
bool health_test_status = true;

// Diagnostic / self-test state
uint8_t selftest_state      = 0;
uint32_t diagnostics_WDT    = 0;

// Protocol / dispatcher status
uint8_t rx_dispatcher_status = 0;

// Transport status
// NOTE:
// Currently logged but otherwise unused.
// Future plan is to centralize transport error reporting here.
uint8_t tx_status = 0;
uint8_t rx_status = 0;


///////////////////////////////////////////////////////////////////////////////
// Temporary / Test Variables
///////////////////////////////////////////////////////////////////////////////

// TEST ONLY
// Remove after diagnostics refactor
bool TEST_call_diag_once = true;


///////////////////////////////////////////////////////////////////////////////
// Utility Functions
///////////////////////////////////////////////////////////////////////////////

// Convert vehicle state enum to readable string
const char* vehicle_state_to_string(VEHICLE_STATE state)
{
    switch (state)
    {
        case VEHICLE_STATE::SAFE_STATE:  return "SAFE_STATE";
        case VEHICLE_STATE::IDLE:        return "IDLE";
        case VEHICLE_STATE::MANUAL:      return "MANUAL";
        case VEHICLE_STATE::AUTONOMOUS:  return "AUTONOMOUS";
        case VEHICLE_STATE::DIAGNOSTICS: return "DIAGNOSTICS";
        case VEHICLE_STATE::FAIL_SAFE:   return "FAIL_SAFE";
        default: return "UNKNOWN";
    }
}