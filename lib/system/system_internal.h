#ifndef SYSTEM_INTERNAL_H
#define SYSTEM_INTERNAL_H

#include <stdint.h>
#include "system_types.h"


/*=============================================================================*
 * System Health
 *=============================================================================*/

extern bool health_test_status;


/*=============================================================================*
 * System Variables
 *=============================================================================*/
namespace sys {

    // Timing 
    extern uint32_t sys_heartbeat;
    extern uint32_t tx_last_valid_packet; 
    extern uint32_t last_connection_attempt;

    // Comunication 
    extern bool communication_warn_active;

    // System status 
    extern bool bus_connectivity_status; 
    extern bool i2c_connectivity_status;

}


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

/*=============================================================================*
 * System Srvice Internal Declerations
 *=============================================================================*/

void update_system_heartbeat();
void update_last_connection_attempt();
void check_transport_alive();  // not currently used in vehicle fsm


#endif