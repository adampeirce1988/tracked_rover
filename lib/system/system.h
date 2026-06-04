#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

// system variables 
extern bool health_test_status;                 // defines if the health_Test has passes or failed
extern uint8_t selftest_state;                  // status code for diagnostic self test 
extern uint32_t diagnostics_WDT;                // WDT diagnostics test timeout

extern uint8_t tx_transport_status;             // stores status code for the transport tx state macheince
extern uint8_t rx_transport_status;             // stores status code for the transport rx state macheince
extern uint8_t rx_protocol_status;              // stores status code for the protocol state macheince

//********** TEST ONLY **********
extern bool TEST_call_diag_once;  // test only code delete once refactored.
//*******************************
  

enum class VEHICLE_STATE{
  SAFE_STATE, 
  IDLE, 
  MANUAL,
  AUTONOMOUS, 
  DIAGNOSTICS, 
  FAIL_SAFE 
};

//decalir variables and set starting vehicle state. 
extern VEHICLE_STATE active_vehicle_state;       // set vehicle to safe state on boot.
extern VEHICLE_STATE last_vehicle_state;        // used for dispalying change only on state change. 
extern VEHICLE_STATE return_state;              // used for retrurning from diagnostics. 

const char* vehicle_state_to_string(VEHICLE_STATE state);

// finite state macheine calls 
void run_vehicle_state();
bool request_vehicle_state_change(VEHICLE_STATE requested);
const char* vehicle_state_to_string(VEHICLE_STATE state);

#endif 