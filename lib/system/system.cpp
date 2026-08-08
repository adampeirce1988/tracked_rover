///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>

#include "system.h"
#include "protocol.h"
#include "global.h"      // TODO: Move sys:: variables into system module
#include "debug.h"
#include "self_test.h"
#include "system_internal.h"


///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////
#define DEBUG_FILE DBG_SYSTEM

///////////////////////////////////////////////////////////////////////////////
// System Status Variables
///////////////////////////////////////////////////////////////////////////////

// Vehicle always boots into BOOTING
VEHICLE_STATE active_vehicle_state = VEHICLE_STATE::BOOTING;

// previous active state this should be diffrent from above.
VEHICLE_STATE last_vehicle_state = VEHICLE_STATE::BOOTING; 

// Used when temporarily entering another state
VEHICLE_STATE return_state  = VEHICLE_STATE::SAFE_STATE;


// Overall system health state
bool health_test_status = true;

// Diagnostic / self-test state
//TEST_RETURN_STATUS selftest_state = TEST_RETURN_STATUS::NO_TEST_RUNNING;
//uint32_t diagnostics_WDT    = 0;

// Protocol / dispatcher status
uint8_t rx_dispacher_status = 0;

// Transport status
// NOTE:
// Currently logged but otherwise unused.
// Future plan is to centralize transport error reporting here.
uint8_t tx_status = 0;
uint8_t rx_status = 0;

///////////////////////////////////////////////////////////////////////////////
// New fuctions
///////////////////////////////////////////////////////////////////////////////

VEHICLE_STATE get_active_vehicle_state(){
    return active_vehicle_state;
}

///////////////////////////////////////////////////////////////////////////////
// Utility Functions
///////////////////////////////////////////////////////////////////////////////

// Convert vehicle state enum to readable string
const char* vehicle_state_to_string(VEHICLE_STATE state)
{
    switch (state)
    {
        case VEHICLE_STATE::BOOTING:        return "BOOTING";
        case VEHICLE_STATE::SAFE_STATE:     return "SAFE_STATE";
        case VEHICLE_STATE::IDLE:           return "IDLE";
        case VEHICLE_STATE::MANUAL:         return "MANUAL";
        case VEHICLE_STATE::AUTONOMOUS:     return "AUTONOMOUS";
        case VEHICLE_STATE::DIAGNOSTICS:    return "DIAGNOSTICS";
        case VEHICLE_STATE::UPDATE:         return "UPDATE";
        case VEHICLE_STATE::FAIL_SAFE:      return "FAIL_SAFE";
        case VEHICLE_STATE::SHUTTING_DOWN:  return "SHUTTING_DOWN";
        default: 
            return "UNKNOWN";
    }
}

// only system alive checks to be added here. 
bool check_system_health_flags(){
    if(sys::bus_connectivity_status && 
        sys::i2c_connectivity_status ){
        return true;
    }  
    else{
        return false;
    }
}

void set_communication_bus_alive(){
    sys::bus_connectivity_status = true; 
}