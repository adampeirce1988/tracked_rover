///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>

#include "system.h"
#include "protocol.h"
#include "global.h"      // TODO: Move sys:: variables into system module
#include "debug.h"
#include "self_test.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////

#define DEBUG_FILE DBG_SYSTEM


///////////////////////////////////////////////////////////////////////////////
// Vehicle State Variables
///////////////////////////////////////////////////////////////////////////////

// Vehicle always boots into SAFE_STATE
VEHICLE_STATE vehicle_state = VEHICLE_STATE::SAFE_STATE;

// Used when temporarily entering another state
VEHICLE_STATE return_state  = VEHICLE_STATE::SAFE_STATE;


///////////////////////////////////////////////////////////////////////////////
// Vehicle State Machine
///////////////////////////////////////////////////////////////////////////////


void run_vehicle_state(){

  switch(vehicle_state){



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
    // DIAGNOSTICS 
    ///////////////////////////////////////////////////////////////////////////
     case VEHICLE_STATE::DIAGNOSTICS:

        // run the fifo engine
        //fifo_io_uart_engine_update(); // later change thsi to only run when FIFO is set in a test. 

        // this will need to be changed to a call from https: 
        if(sys::diagnostics_active == false){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "diag_active change to true.");
        // this function will need to be called by the web interface once built.
        sys::diagnostics_active = request_self_test(1); // request test 1 - 3 as this is the only test at present
        }
        
        selftest_state = run_test_case();

        if(selftest_state == SELFTEST_COMPLETED){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "SELF_TEST COMPLETED!.");
        update_last_connection_attempt();            //update the hearbeat before exiting to prvent false triger of safe
        request_vehicle_state_change(VEHICLE_STATE::IDLE);  // return to the previous state 
        }
        
        //WDT timer check for diagnostics runs for total test time ensure this id longer than the longest test of calibration. 
        if(millis() - diagnostics_WDT > DIAGNOSTIC_WT_TIMEOUT_MS){
            if(watchdog_test_active()){
                ST_LOG_EVENT(EVENT_WDT_TRIGERED);
                diagnostics_WDT += ST_WDT_EXTRA_TIME; 
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WDT", "event_wdt_trigered activated.");
                break;
            }
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::DIAGNOSTICS Watchdog timer triggered.");
        request_vehicle_state_change(VEHICLE_STATE::IDLE);
        }
        break;
    
    ///////////////////////////////////////////////////////////////////////////
    // DEFAULT
    ///////////////////////////////////////////////////////////////////////////
    default:
        vehicle_state = VEHICLE_STATE::SAFE_STATE; 
    break; 
  };
}