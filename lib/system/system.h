#ifndef SYSTEM_H
#define SYSTEM_H

#include "system_types.h"


/*=============================================================================*
 * System API
 *=============================================================================*/

// Run the vehicle state machine.
VEHICLE_STATE_RETURN_CODE run_vehicle_state();

// Run core functions regardless of vehicle state.
void run_core_functions();

// Return the current active vehicle state.
VEHICLE_STATE get_active_vehicle_state();


/*=============================================================================*
 * System Health
 *=============================================================================*/

// Check current system health flags.
bool check_system_health_flags();

// Set communication bus alive flag.
void set_communication_bus_alive();

/*=============================================================================*
 * System services API
 *=============================================================================*/

// Update the timestamp of the last valid communication packet.
void update_last_valid_comms(); 


#endif