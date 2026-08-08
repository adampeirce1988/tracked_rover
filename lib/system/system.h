#ifndef SYSTEM_H
#define SYSTEM_H


#include <stdint.h>
#include "system_types.h"
#include "transport.h" // TODO: remove if no longer required


/*=============================================================================*
 * Vehicle State Return Code
 *=============================================================================*/

enum class VEHICLE_STATE_RETURN_CODE : uint8_t
{
    RUNNING,
    CHANGE_DENIED,
    CHANGE_APPROVED
};


/*=============================================================================*
 * System API
 *=============================================================================*/

// Run the vehicle state machine.
VEHICLE_STATE_RETURN_CODE run_vehicle_state();

// Run core functions regardless of vehicle state.
void run_core_functions();

// External access to the current vehicle state.
VEHICLE_STATE get_active_vehicle_state();


/*=============================================================================*
 * System Health
 *=============================================================================*/

// Check system health flags.
bool check_system_health_flags();

// Set communication bus alive flag.
void set_communication_bus_alive();


#endif