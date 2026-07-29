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
#include "transport.h"
#include "simulation.h"



///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////

#define DEBUG_FILE DBG_SYSTEM

///////////////////////////////////////////////////////////////////////////////
// Temporary / Test Variables
///////////////////////////////////////////////////////////////////////////////

// TEST ONLY
// Remove after diagnostics refactor
bool TEST_call_diag_once = true;

///////////////////////////////////////////////////////////////////////////////
// Vehicle State Variables
///////////////////////////////////////////////////////////////////////////////

// Vehicle always boots into BOOTING
VEHICLE_STATE active_vehicle_state = VEHICLE_STATE::BOOTING;

// previous active state this should be diffrent from above.
VEHICLE_STATE last_vehicle_state = VEHICLE_STATE::BOOTING; 

// Used when temporarily entering another state
VEHICLE_STATE return_state  = VEHICLE_STATE::SAFE_STATE;


///////////////////////////////////////////////////////////////////////////////
// Vehicle State Machine
///////////////////////////////////////////////////////////////////////////////


VEHICLE_STATE_RETURN_CODE run_vehicle_state(){

    
  run_core_functions(); // this line runs all the core functions that need to run in all states 
  

  switch(active_vehicle_state){

    //setup code will go here not in main.cpp void setup();
    case VEHICLE_STATE::BOOTING: 

      // open debug port and print version data.
      debug_port_begin();                                                    // open the debug port
      delay(1000);                                                           // run 1s delay before transmitting data 
      PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES);       // print version and meta data 
      delay(1000);

      // set the current transport & baud rate. 
      //transport_set(&uart_io); ** remove after refactor ** 
      transport_set_default();                                                  // set transport method default to be serial (&uart_io / &fifo_io)
      ports_init(COMS_PORT_BAUD);                                           // open inter board serial port 
      
      // **DELETE** once handled via the web interface
      user_enable_simulation();  //***** DELETE ONCE HANDLED BY THE WEB INTERFACE ***** 
      

      // configure wifi handle ap fall back. 
      //WiFi.mode(WIFI_AP);

      // leave booting always transition to SAFE_STATE
      request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);               // move to safe state. 
    break;

    ///////////////////////////////////////////////////////////////////////////
    // SAFE STATE
    ///////////////////////////////////////////////////////////////////////////
     case VEHICLE_STATE::SAFE_STATE:

      
      //send out a request for confirmation of receiving node ** THIS ONLY EVER RUNS ONCE **
      if(!sys::bus_connectivity_status && millis() - sys::last_connection_attempt > 1000){
        update_last_connection_attempt(); 
        establish_coms();
      }
      
      // check all system flags move to idle
      if(check_system_health_flags()){ // I2C check not needed for ESP. Arduino will return an error if the bus is offline.
        request_vehicle_state_change(VEHICLE_STATE::IDLE);
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE: ", vehicle_state_to_string(active_vehicle_state) );
      } 
      else{
    
      // no commands are accepted or executed. 
      // I2C bus and sensor communication will stop. rund device check to confirm the bus status then halt transmission 
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
      request_self_test(TEST_ID::TRANSPORT_TYPE_CHANGE);
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

    case VEHICLE_STATE::UPDATE: 

      // Access via the OTA updater tab 
      // used only to carry out:
      //   - ESP32 OTA update
      //   - UX HTML file
      //   - ARDUINO via SDK passthrough (Much later on in the project)
      // possible to exit by changing to another tab without proforming a firmware update

    break; 
    
    ///////////////////////////////////////////////////////////////////////////
    // DEFAULT
    ///////////////////////////////////////////////////////////////////////////
    default:
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE; 
    break; 
  };

  return VEHICLE_STATE_RETURN_CODE::RUNNING; // defult until full implimentaion 
}
