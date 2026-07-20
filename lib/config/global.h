#ifndef GLOBAL_H
#define GLOBAL_H


#include <stdint.h>

void update_system_heartbeat();
void update_last_valid_comms();
void update_last_connection_attempt();
//bool check_system_health_flags();
void check_transport_alive();

/////////////////////////////////////////
//          SYSTEM GLOBALS             // 
/////////////////////////////////////////

namespace sys {

    // timing 
    extern uint32_t sys_heartbeat;
    extern uint32_t tx_last_valid_packet; 
    extern uint32_t last_connection_attempt;

    // comunication 
    extern bool communication_warn_active;

    // system status 
    extern bool bus_connectivity_status; 
    extern bool i2c_connectivity_status;

    // modes
    extern bool diagnostics_active;

}


// function declerations 
void update_system_heartbeat();
void update_last_valid_comms();
void update_last_connection_attempt();

void check_transport_alive();  // not currently used in vehicle fsm
bool check_system_health_flags(); 


#endif 