#ifndef SYSTEM_INTERNAL_H
#define SYSTEM_INTERNAL_H


#include "system_types.h"


/*=============================================================================*
 * System Health
 *=============================================================================*/

extern bool health_test_status;


/*=============================================================================*
 * Vehicle State Machine
 *=============================================================================*/

extern VEHICLE_STATE active_vehicle_state;
extern VEHICLE_STATE last_vehicle_state;
extern VEHICLE_STATE return_state;


/*=============================================================================*
 * Vehicle State Machine Functions
 *=============================================================================*/

bool request_vehicle_state_change(VEHICLE_STATE requested);

const char* vehicle_state_to_string(VEHICLE_STATE state);


#endif