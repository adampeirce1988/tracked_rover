
// TODO: on return to safe_state the paramer sys::comunication esatbolished to false on error. 

#include <Arduino.h>
#include <HardwareSerial.h>
#include "self_test.h"
#include "log.h"
#include "transport.h"
#include "debug.h"
#include "protocol.h"
#include "global_config.h"
#include "global.h"
#include "messages.h"
#include "system.h"


#define DEBUG_FILE DBG_ESP_MAIN

// void run_vehicle_state();
// bool request_vehicle_state_change(VEHICLE_STATE requested);


// // system variables 
// bool health_test_status = true;    // defines if the health_Test has passes or failed
// uint8_t selftest_state = 0;        // status code for diagnostic self test 
// uint32_t diagnostics_WDT = 0;      // WDT diagnostics test timeout
// uint8_t rx_dispacher_status = 0;   // status code for the protocol state macheince

// // these values are currnetly logged but unused the need to be used to log errors 
// // this functionality can then be used to remove logging from transport. 
// uint8_t tx_status = 0; 
// uint8_t rx_status = 0; 

// //decalir variables and set starting vehicle state. 
// VEHICLE_STATE vehicle_state = VEHICLE_STATE::SAFE_STATE;               // set vehicle to safe state on boot.
// VEHICLE_STATE return_state = VEHICLE_STATE::SAFE_STATE;                // store the state to return to once diagnostics is complete.     

void setup(){
  // open debug port and print version data. 
  debug_port_begin();                                                    // open the debug port
  delay(3000);                                                           // run 1s delay before transmitting data 
  PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES);       // print version and meta data 

  // set the current transport & baud rate. 
  transport_set(&fifo_io);                                               // set transport method default to be serial (&uart_io / &fifo_io)
  coms_port_begin(COMS_PORT_BAUD);                                       // start the port at baud rate defined in gloabl_config.h (default 115200)

}

void loop() {

  DEBUG_PORT.println(vehicle_state_to_string(vehicle_state));

  // update core systems 
  rx_status = update_rx_fsm(); 
  tx_status = update_tx_fsm();

  // check for avaliable packets and run the dispacher
  if(frame_avaliable() > 0){
    rx_dispacher_status = rx_message_task_dispatcher();
    if(rx_dispacher_status == false){
      DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "MAIN", "message recived with an invalid type. type: " , missmatche_type_received);
    }
  }


  fifo_io_uart_engine_update();    // this only needs to run if transport diagnostics occours. 
  

  // run vehicle requested FSM. 
  run_vehicle_state();
  //DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "main", "vehicel_state: ", sys::verbous_current_state); 
}


//*******************************************************************************************/
//*******************************************************************************************/


// //********** TEST ONLY **********
// bool TEST_call_diag_once = true; 
// //*******************************


// void run_vehicle_state(){

//   switch(vehicle_state){

//     case VEHICLE_STATE::SAFE_STATE:

//       //send out a request for confirmation of receiving node
//       if(!sys::bus_connectivity_status && millis() - sys::last_connection_attempt > 1000){
//         update_last_connection_attempt(); 
//         establish_coms();
//       }
      
//       // check all system flags move to idle
//       if(check_system_health_flags()){ // I2C check not needed for ESP. Arduino will return an error if the bus is offline.
//         request_vehicle_state_change(VEHICLE_STATE::IDLE);
//       } 
//       else{
//       // no commands are accepted or executed. 
//       // I2C bus and sensor communication will stop.
//       // any errors that caused the safe state to occur will be logged.
//       break;
//       }
//     break;

//     case VEHICLE_STATE::IDLE:

//       // on request for diagnostics a test case must be requested 
//       if(TEST_call_diag_once == true){
//       DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "diagnostics called.");
//       request_vehicle_state_change(VEHICLE_STATE::DIAGNOSTICS);
//       TEST_call_diag_once = false;
//       }


//       // comunication established and all buses live.
//       // heartbeat monitored and upated. 
//       // able to select: 
//       //    running
//       //    SW_update
//       //    Diagnostic self tests. 
//       //    retrieve error & data logs

//       //go straight to manule when manule inputs are received // this code will run on arduino 

//       if(!check_system_health_flags()){request_vehicle_state_change(VEHICLE_STATE::SAFE_STATE);}
//       break;

//       case VEHICLE_STATE::MANUAL: 
//       // operational commands accepted and vehicle drivable and any of control signals > 0
//       // return back to idle once no commands are received for xxMS
//     break;

//           case VEHICLE_STATE::DIAGNOSTICS:

//           // run the fifo engine
//           //fifo_io_uart_engine_update(); // later change thsi to only run when FIFO is set in a test. 

//           // this will need to be changed to a call from https: 
//           if(sys::diagnostics_active == false){ 
//             DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "diag_active change to true.");
//             // this function will need to be called by the web interface once built.
//             sys::diagnostics_active = request_self_test(1); // request test 1 - 3 as this is the only test at present
//           }
          
//           selftest_state = run_test_case();

//           if(selftest_state == SELFTEST_COMPLETED){
//             DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "SELF_TEST COMPLETED!.");
//             update_last_connection_attempt();            //update the hearbeat before exiting to prvent false triger of safe
//             request_vehicle_state_change(VEHICLE_STATE::IDLE);  // return to the previous state 
//           }
          
//           //WDT timer check for diagnostics runs for total test time ensure this id longer than the longest test of calibration. 
//           if(millis() - diagnostics_WDT > DIAGNOSTIC_WT_TIMEOUT_MS){
//             if(watchdog_test_active()){
//               ST_LOG_EVENT(EVENT_WDT_TRIGERED);
//               diagnostics_WDT += ST_WDT_EXTRA_TIME; 
//               DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WDT", "event_wdt_trigered activated.");

//               break;
//             }
//             DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::DIAGNOSTICS Watchdog timer triggered.");
//             request_vehicle_state_change(VEHICLE_STATE::IDLE);
//           }
//           break;

//     default:
//       vehicle_state = VEHICLE_STATE::SAFE_STATE; 
//     break; 
//   };
// }


// //*******************************************************************************************/
// //*******************************************************************************************/


// bool request_vehicle_state_change(VEHICLE_STATE requested){

//   switch(vehicle_state){

//     case VEHICLE_STATE::SAFE_STATE:
//       if(requested == VEHICLE_STATE::SAFE_STATE){
//         sys::verbous_current_state = "SAFE_STATE";
//       }
//       else if(requested == VEHICLE_STATE::IDLE && health_test_status == true){
//         vehicle_state = VEHICLE_STATE::IDLE;
//         sys::verbous_current_state = "IDLE";
//       }
//       else if(requested == VEHICLE_STATE::DIAGNOSTICS){
//         return_state = VEHICLE_STATE::SAFE_STATE;
//         vehicle_state = VEHICLE_STATE::DIAGNOSTICS;
//         sys::verbous_current_state = "DIAGNOSTICS";
//         diagnostics_WDT = millis();
//       }
//       else{
//         // do nothing as vehicle still in safe stat
//         return false;
//       }
//       return true; 
//     break; 

//     case VEHICLE_STATE::IDLE:
//       if(requested == VEHICLE_STATE::SAFE_STATE){
//         DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::SAFE_STATE requested from IDLE.");
//         vehicle_state = VEHICLE_STATE::SAFE_STATE; 
//         sys::verbous_current_state = "SAFE_STATE";
//       }
//       else if(requested == VEHICLE_STATE::MANUAL){
//         vehicle_state = VEHICLE_STATE::MANUAL;
//         sys::verbous_current_state = "MANUAL"; 
//       }
//       else if(requested == VEHICLE_STATE::DIAGNOSTICS){
//         return_state = VEHICLE_STATE::IDLE; 
//         vehicle_state = VEHICLE_STATE::DIAGNOSTICS; 
//         sys::verbous_current_state = "DAIGNOSTICS"; 
//         diagnostics_WDT = millis();
//       }
//       else{
//         DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::IDLE not avaliable form the current state");
//         return false; 
//       }
//     return true;   
//     break; 

//       case VEHICLE_STATE::MANUAL:
//         if(requested == VEHICLE_STATE::SAFE_STATE){
//           vehicle_state = VEHICLE_STATE::SAFE_STATE;
//           sys::verbous_current_state = "SAFE_STATE";
//         }
//         else if(requested == VEHICLE_STATE::IDLE){
//           vehicle_state = VEHICLE_STATE::IDLE; 
//           sys::verbous_current_state = "IDLE"; 
//         }
//         else{
//           DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::MANUALE not avaliable form the current state");
//           return false; 
//         }
//         return true;
//       break; 

//       case VEHICLE_STATE::AUTONOMOUS:
//         DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::AUTONOMOUS currently unsupported");
//         return false; 
//       break; 

//       case VEHICLE_STATE::DIAGNOSTICS:  // this need looking at as its not possible return a state.

//         if(requested == VEHICLE_STATE::SAFE_STATE && return_state == VEHICLE_STATE::SAFE_STATE){
//           DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN",  "DIAGNOSTICS returnung to VEHICLE_STATE::SAFE_STATE.");
//           vehicle_state = return_state; 
//           return_state = VEHICLE_STATE::SAFE_STATE;  
//         }
//         else if(requested == VEHICLE_STATE::IDLE && return_state == VEHICLE_STATE::IDLE){
//           DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN",  "DIAGNOSTICS returnung to VEHICLE_STATE::IDLE.");
//           vehicle_state = return_state; 
//           return_state = VEHICLE_STATE::SAFE_STATE; 
        
//         }
//         else{
//           DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: unable to request current state transition form diagnostics.");
//           return false; 
//         }
//         return true; 
//       break; 

//       case VEHICLE_STATE::FAIL_SAFE:
//         DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::FAIL_SAFE activated"); 

//         // reset all valuse here hen return to safe
//         // continue to opperte no commands to be actiones. 

//         return false; 
//       break; 

//     break;
//   }
//   return false; 
// }
