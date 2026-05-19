#ifndef SYSTEM_H
#define SYSTEM_H



// system variables 
extern bool health_test_status;    // defines if the health_Test has passes or failed
extern uint8_t selftest_state;        // status code for diagnostic self test 
extern uint32_t diagnostics_WDT;      // WDT diagnostics test timeout
extern uint8_t rx_dispacher_status;   // status code for the protocol state macheince

//********** TEST ONLY **********
extern bool TEST_call_diag_once;  // test only code delete once refactored.
//*******************************

// these values are currnetly logged but unused the need to be used to log errors 
// this functionality can then be used to remove logging from transport. 
extern uint8_t tx_status; 
extern uint8_t rx_status; 

  

enum class VEHICLE_STATE{
  SAFE_STATE, 
  IDLE, 
  MANUAL,
  AUTONOMOUS, 
  DIAGNOSTICS, 
  FAIL_SAFE 
};

//decalir variables and set starting vehicle state. 
extern VEHICLE_STATE vehicle_state;             // set vehicle to safe state on boot.
extern VEHICLE_STATE return_state;

const char* vehicle_state_to_string(VEHICLE_STATE state);

// finite state macheine calls 
void run_vehicle_state();
bool request_vehicle_state_change(VEHICLE_STATE requested);

#endif 