/*=============================================================================*
 * Includes
*=============================================================================*/

#include <Arduino.h>

#include "system.h"
#include "system_internal.h"
#include "protocol.h"
#include "debug.h"
#include "self_test.h"
#include "transport.h"
#include "simulation.h"


/*=============================================================================*
 * Configuration
*=============================================================================*/

#define DEBUG_FILE DBG_SYSTEM


/*=============================================================================*
 * Temporary / Test Variables
*=============================================================================*/

// TEST ONLY
// Remove after diagnostics refactor
bool TEST_call_diag_once = true;


/*=============================================================================*
 * Vehicle State Machine
 *=============================================================================*/

VEHICLE_STATE_RETURN_CODE run_vehicle_state(){

  // This line runs all the core functions that need to run in all states 
  run_core_functions(); 
  

  switch(active_vehicle_state){


    /*-------------------------------------------------------------------------*
      * BOOTING
      
      * System initialisation is performed here rather than in main.cpp.
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::BOOTING: 

      // Open debug port and print version data.
      debug_port_begin();                                                    // open the debug port
      delay(1000);                                                           // run 1s delay before transmitting data 
      PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES);       // print version and metadata 
      delay(1000);

      // Configure the default transport and baud rate.
      transport_set_default();                                                  // set transport method default to be serial (&uart_io / &fifo_io)
      transport_init(COMS_PORT_BAUD);                                           // open inter-board serial port 
      
      // **DELETE** once handled via the web interface
      user_enable_simulation();  //***** DELETE ONCE HANDLED BY THE WEB INTERFACE ***** 
      

      // Configure WiFi/AP fallback
      //WiFi.mode(WIFI_AP);

      // leave booting always transition to SAFE_STATE
      request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);
    break;

      /*-------------------------------------------------------------------------*
        * SAFE STATE
      *-------------------------------------------------------------------------*/
     case VEHICLE_STATE::SAFE_STATE:

      
      // Attempt to establish communication once per second until the communication bus is active.
      if(!sys::bus_connectivity_status && millis() - sys::last_connection_attempt > 1000){
        update_last_connection_attempt(); 
        establish_coms();
      }
      
      // Transition to IDLE once all required system health checks pass.
      if(check_system_health_flags()){ // I2C check not needed for ESP. Arduino will return an error if the bus is offline.
        request_vehicle_state_change(VEHICLE_STATE::IDLE);
      } 
      else{

      // TODO: 
      // no commands are accepted or executed. 
      // I2C bus and sensor communication will stop. run device check to confirm the bus status then halt transmission 
      // any errors that caused the safe state to occur will be logged.

      break;
      }

    break;

    /*-------------------------------------------------------------------------*
      * IDLE
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::IDLE:

      // ** TEST ONLY **
      // Remove when diagnostic entry is controlled by the system/web interface.
      // on request for diagnostics a test case must be requested 
      if(TEST_call_diag_once){
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "diagnostics called.");
      request_self_test(TEST_ID::TRANSPORT_TYPE_CHANGE);
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

      
    /*-------------------------------------------------------------------------*
      * MANUAL
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::MANUAL: 
      // Vehicle operational
      // Accept live control commands

      // TODO:
      // Return to IDLE after timeout
      // if no commands received
    break;

    /*-------------------------------------------------------------------------*
      * AUTONOMOUS
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::AUTONOMOUS: 
      // Vehicle operation in autonomous mode
      // Not yet implemented
    break;

    /*-------------------------------------------------------------------------*
      * DIAGNOSTICS
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::DIAGNOSTICS:

    // Only run the diagnostics FSM here all other functions are handled by the FSM core. 
    run_test_manager();
        
    break;

    /*-------------------------------------------------------------------------*
      * UPDATE 
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::UPDATE: 

      // OTA update services:
      // - ESP32 firmware
      // - Web interface files
      // - Arduino firmware passthrough

    break; 

    /*-------------------------------------------------------------------------*
      * FAIL SAFE 
    *-------------------------------------------------------------------------*/

    case VEHICLE_STATE::FAIL_SAFE: 
      
    // Fault detected the vehicle will be stopped and made safe
    // (may later only operate in derate depending on the issue)
    request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);

    break; 


    /*-------------------------------------------------------------------------*
      * SHUTTING DOWN
    *-------------------------------------------------------------------------*/
    case VEHICLE_STATE::SHUTTING_DOWN:

     // Save and close all open files
     // enter low power mode and wait for power disconnection or rebooting command 

    break; 
    
    /*-------------------------------------------------------------------------*
      * DEFAULT
    *-------------------------------------------------------------------------*/
    default:
        request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);
    break; 
  };

  // Default until the state machine has meaningful return conditions.
  return VEHICLE_STATE_RETURN_CODE::RUNNING; 
}
