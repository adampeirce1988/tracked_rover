///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>

#include "system.h"
#include "protocol.h"
#include "global.h"      // TODO: Move sys:: variables into system module
#include "debug.h"
#include "logger.h"
#include "self_test.h"


///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////

#define DEBUG_FILE DBG_SYSTEM


///////////////////////////////////////////////////////////////////////////////
// Vehicle State Variables
///////////////////////////////////////////////////////////////////////////////

// Vehicle always boots into SAFE_STATE
VEHICLE_STATE active_vehicle_state = VEHICLE_STATE::SAFE_STATE;

// previous active state this should be diffrent from above.
VEHICLE_STATE last_vehicle_state = VEHICLE_STATE::FAIL_SAFE; 

// Used when temporarily entering another state
VEHICLE_STATE return_state  = VEHICLE_STATE::SAFE_STATE;


///////////////////////////////////////////////////////////////////////////////
// Vehicle State Machine
///////////////////////////////////////////////////////////////////////////////


void run_vehicle_state(){

  switch(active_vehicle_state){

    ///////////////////////////////////////////////////////////////////////////
    // SAFE STATE
    ///////////////////////////////////////////////////////////////////////////
     case VEHICLE_STATE::SAFE_STATE:

      //send out a request for confirmation of receiving node
      if(!sys::bus_connectivity_status && millis() - sys::last_connection_attempt > 1000){
        update_last_connection_attempt(); 
        establish_coms();
      }
      
      // check all system flags move to idle
      if(check_system_health_flags()){ // I2C check not needed for ESP. Arduino will return an error if the bus is offline.
        request_vehicle_state_change(VEHICLE_STATE::IDLE);
      } 
      else{
      // no commands are accepted or executed. 
      // I2C bus and sensor communication will stop.
      // any errors that caused the safe state to occur will be logged.
      break;
      }
    break;

    ///////////////////////////////////////////////////////////////////////////
    // IDLE
    ///////////////////////////////////////////////////////////////////////////
    case VEHICLE_STATE::IDLE:

      // on request for diagnostics a test case must be requested 
      if(TEST_call_diag_once == true){
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "diagnostics called.");
      request_vehicle_state_change(VEHICLE_STATE::DIAGNOSTICS);
      TEST_call_diag_once = false;
      }

        // System status:
        // - Communications established
        // - Heartbeat monitored
        // - Awaiting commands
        //
        // Available operations:
        // - Manual control
        // - Software updates
        // - Diagnostic tests
        // - Log retrieval

        // TODO:
        // Transition directly to MANUAL
        // when valid control inputs are received

      // Health monitoring
      if(!check_system_health_flags()){request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);}
    break;

      
    ///////////////////////////////////////////////////////////////////////////
    // MANUAL
    ///////////////////////////////////////////////////////////////////////////
    case VEHICLE_STATE::MANUAL: 
      // Vehicle operational
      // Accept live control commands

      // TODO:
      // Return to IDLE after timeout
      // if no commands received
    break;


    ///////////////////////////////////////////////////////////////////////////
    // AUTONOMOUS
    ///////////////////////////////////////////////////////////////////////////
    case VEHICLE_STATE::AUTONOMOUS: 
      // Vehicle operation in atonomuse mode
      // Not yet implimented
    break;

    
    ///////////////////////////////////////////////////////////////////////////
    // DIAGNOSTICS 
    ///////////////////////////////////////////////////////////////////////////
    case VEHICLE_STATE::DIAGNOSTICS:

    //** only run the diagnostic FSM here all other functionsa to be handled by the FSM. 
    // convert return code to unit8_t and use test_manager_status = run_test_manager();
    run_test_manager();
        
    break;
    
    ///////////////////////////////////////////////////////////////////////////
    // DEFAULT
    ///////////////////////////////////////////////////////////////////////////
    default:
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE; 
    break; 
  };
}
