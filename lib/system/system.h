#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

// system variables 
extern bool health_test_status;                 // defines if the health_Test has passes or failed

//extern TEST_RETURN_STATUS selftest_state;     // status code for diagnostic self test 
extern uint32_t diagnostics_WDT;                // WDT diagnostics test timeout

// store module return codes 
extern uint8_t tx_transport_status;             // stores status code for the transport tx state macheince
extern uint8_t rx_transport_status;             // stores status code for the transport rx state macheince
extern uint8_t rx_protocol_status;              // stores status code for the protocol state macheince
extern uint8_t test_manager_status;             // stores status code for the self test manager


enum class VEHICLE_STATE{
  BOOTING,          // all setup code will run ib here and not in main. 
  SAFE_STATE,       // allow communication recovery messages only & block all control and motor commands
  IDLE,             // vehicle idle awaiting commands 
  MANUAL,           // vehile under opporators control 
  AUTONOMOUS,       // vehicle oppeerates autonomusly (this will be the last to impliment)
  DIAGNOSTICS,      // Run on boad self tests and live data. 
  UPDATE,           // carry out an OTA update of the ESP 
  FAIL_SAFE,        // fault detected the vehicle will be stoped (may later only opperate in derate depending on the issue)
  SHUTING_DOWN      // used to shut down the esp (all files that are open on LittleFS will be closed and saved) 
};

enum class VEHICLE_STATE_RETURN_CODE{ 
  RUNNING, 
  CHANGE_DENIED,
  CHANGE_APPROVED
}; 

// decalir variables and set starting vehicle state. 
extern VEHICLE_STATE active_vehicle_state;      // set vehicle to safe state on boot.
extern VEHICLE_STATE last_vehicle_state;        // used for dispalying change only on state change. 
extern VEHICLE_STATE return_state;              // used for retrurning from diagnostics. 

// Function prototypes 
VEHICLE_STATE_RETURN_CODE run_vehicle_state();
bool request_vehicle_state_change(VEHICLE_STATE requested);
const char* vehicle_state_to_string(VEHICLE_STATE state);
void run_core_functions(); 


// moved from global.h 
bool check_system_health_flags();

#endif 