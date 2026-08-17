#include "system.h"
#include "system_internal.h"


/*=============================================================================*
 * System Runtime Variables
 *=============================================================================*/

// vehicle states
VEHICLE_STATE active_vehicle_state  = VEHICLE_STATE::BOOTING;
VEHICLE_STATE last_vehicle_state    = VEHICLE_STATE::BOOTING; 
VEHICLE_STATE return_state          = VEHICLE_STATE::SAFE_STATE;

//  System health flags
bool health_test_status = true; // Review usage once the web application is implemented.


// System communication state
namespace sys
{
    uint32_t sys_heartbeat = 0;
    uint32_t tx_last_valid_packet = 0;
    uint32_t last_connection_attempt = 0;

    bool communication_warn_active = false;

    bool bus_connectivity_status = false;
    bool i2c_connectivity_status = true;

}


/*=============================================================================*
 * Public Accessors
 *=============================================================================*/

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
