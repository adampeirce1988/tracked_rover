
// TODO: on return to safe_state the paramer sys::comunication esatbolished to false on error. 

#include <Arduino.h>
#include <HardwareSerial.h>
#include "self_test.h"
#include "transport.h"
#include "debug.h"
#include "protocol.h"
#include "global_config.h"
#include "global.h"
#include "messages.h"


#define DEBUG_FILE DBG_ESP_MAIN

enum class VEHICLE_STATE{
  SAFE_STATE, 
  IDLE, 
  MANUAL,
  AUTONOMOUS, 
  DIAGNOSTICS, 
  EXIT_DIAGNOSTICS, 
  FAIL_SAFE 
};

void run_vehicle_state();
bool request_vehicle_state_change(VEHICLE_STATE requested);

// system variables 
bool system_alive = true;
bool selftest_state = false;  

uint32_t diagnostics_WDT = 0; 

//decalir variables and set starting vehicle state. 
VEHICLE_STATE vehicle_state = VEHICLE_STATE::SAFE_STATE;               // set vehicle to safe state on boot.
VEHICLE_STATE return_state = VEHICLE_STATE::SAFE_STATE;                // store the state to return to once diagnostics is complete.     


uint8_t tx_status = 0; 
uint8_t rx_status = 0; 


void setup(){
  // open debug port and print version data. 
  debug_port_begin();                                                    // open the debug port
  delay(1000);                                                           // run 1s delay before transmitting data 
  PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES);       // print version and meta data 

  // set the current transport & baud rate. 
  transport_set(&fifo_io);                                               // set transport method default to be serial (&uart_io / &fifo_io)
  coms_port_begin(COMS_PORT_BAUD);                                       // start the port at baud rate defined in gloabl_config.h (default 115200)

}

void loop() {

  // update core systems 
  rx_status = update_rx_fsm();
  tx_status = update_tx_fsm();
  fifo_io_uart_engine_update();    // this only needs to run if transport diagnostics occours. 
  
  // run vehicle requested FSM. 
  run_vehicle_state();
  //DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "main", "vehicel_state: ", sys::verbous_current_state); 
}


//*******************************************************************************************/
//*******************************************************************************************/


void run_vehicle_state(){

  switch(vehicle_state){

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

    case VEHICLE_STATE::IDLE:

      // on request for diagnostics a test case must be requested 
      request_vehicle_state_change(VEHICLE_STATE::DIAGNOSTICS);



      // comunication established and all buses live.
      // heartbeat monitored and upated. 
      // able to select: 
      //    running
      //    SW_update
      //    Diagnostic self tests. 
      //    retrieve error & data logs

      //
      if(!check_system_health_flags()){request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);}
      break;
      case VEHICLE_STATE::MANUAL: 

      // operational commands accepted and vehicle drivable
    break;

                        case VEHICLE_STATE::DIAGNOSTICS:
                        
                        selftest_state = run_test_case();

                        if(selftest_state == true ){ // this line has an error changd from SELFTEST_ENDED to true as a temp solution. 
                          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "self-test completed.");
                        }

                        // WDT timer check for diagnostics runs for total test time ensure this id longer than the longest test of calibration. 
                        if(millis() - diagnostics_WDT > DIAGNOSTIC_WT_TIMEOUT_MS){
                          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::DIAGNOSTICS Watchdog timer triggered.");
                          request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);
                        }

                        //self-tests and calibrations can be run in ths requested
                        // heartbeat is not monitored but checked & confirmed before switching back to pre running. 
                        break;

    case VEHICLE_STATE::EXIT_DIAGNOSTICS: 
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "None usable state called VEHICLE_STATE::EXIT_DIAGNOSTICS.");
      request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);
    break; 

    default:
      vehicle_state = VEHICLE_STATE::SAFE_STATE; 
    break; 
  };
}


//*******************************************************************************************/
//*******************************************************************************************/


bool request_vehicle_state_change(VEHICLE_STATE requested){

  switch(vehicle_state){

    case VEHICLE_STATE::SAFE_STATE:
      if(requested == VEHICLE_STATE::SAFE_STATE){
        sys::verbous_current_state = "SAFE_STATE";
      }
      else if(requested == VEHICLE_STATE::IDLE && system_alive == true){
        vehicle_state = VEHICLE_STATE::IDLE;
        sys::verbous_current_state = "IDLE";
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::SAFE_STATE;
        vehicle_state = VEHICLE_STATE::DIAGNOSTICS;
        sys::verbous_current_state = "DIAGNOSTICS";
        diagnostics_WDT = millis();
      }
      else{
        // do nothing as vehicle still in safe stat
        return false;
      }
      return true; 
    break; 

    case VEHICLE_STATE::IDLE:
      if(requested == VEHICLE_STATE::SAFE_STATE){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::SAFE_STATE requested from IDLE.");
        vehicle_state = VEHICLE_STATE::SAFE_STATE; 
        sys::verbous_current_state = "SAFE_STATE";
      }
      else if(requested == VEHICLE_STATE::MANUAL){
        vehicle_state = VEHICLE_STATE::MANUAL;
        sys::verbous_current_state = "MANUAL"; 
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::IDLE; 
        vehicle_state = VEHICLE_STATE::DIAGNOSTICS; 
        sys::verbous_current_state = "DAIGNOSTICS"; 
        diagnostics_WDT = millis();
      }
      else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::IDLE not avaliable form the current state");
        return false; 
      }
    return true;   
    break; 

      case VEHICLE_STATE::MANUAL:
        if(requested == VEHICLE_STATE::SAFE_STATE){
          vehicle_state = VEHICLE_STATE::SAFE_STATE;
          sys::verbous_current_state = "SAFE_STATE";
        }
        else if(requested == VEHICLE_STATE::IDLE){
          vehicle_state = VEHICLE_STATE::IDLE; 
          sys::verbous_current_state = "IDLE"; 
        }
        else{
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::MANUALE not avaliable form the current state");
          return false; 
        }
        return true;
      break; 

      case VEHICLE_STATE::AUTONOMOUS:
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::AUTONOMOUS currently unsupported");
        return false; 
      break; 

      case VEHICLE_STATE::DIAGNOSTICS:  // this need looking at as its not possible return a state.
        if(requested == VEHICLE_STATE::EXIT_DIAGNOSTICS){
         vehicle_state = return_state; 
         return_state = VEHICLE_STATE::SAFE_STATE;
         break; 
        }
        else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: unable to request state transition form diagnostics.");
        return false; 
        }
        return true; 
      break; 

      case VEHICLE_STATE::EXIT_DIAGNOSTICS: 
        vehicle_state = return_state;
      break; 

      case VEHICLE_STATE::FAIL_SAFE:
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::FAIL_SAFE activated"); 

        // reset all valuse here hen return to safe
        // continue to opperte no commands to be actiones. 

        return false; 
      break; 

    break;
  }
  return false; 
}
