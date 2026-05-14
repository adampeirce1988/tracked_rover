#ifndef GLOBAL_H
#define GLOBAL_H

#pragma once

#include <stdint.h>

void update_system_heartbeat();
void update_last_valid_comms();
void update_last_connection_attempt();
bool check_system_health_flags();


/////////////////////////////////////////
//          SYSTEM GLOBALS             // 
/////////////////////////////////////////

namespace sys {

extern const char* verbous_current_state;

extern uint32_t sys_heartbeat;
extern uint32_t tx_last_valid_packet; 
extern uint32_t last_connection_attempt;
extern bool bus_connectivity_status; 
extern bool i2c_connectivity_status;

// system alive flags.
extern bool I2C_bus_alive;
extern bool tranport_alive; 

// error states
extern bool I2C_bus_error;
}

#endif 