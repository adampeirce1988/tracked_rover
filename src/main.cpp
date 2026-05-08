
#include <Arduino.h>
#include <HardwareSerial.h>
#include "self_test.h"
#include "transport.h"
#include "debug.h"
#include "global_config.h"
#include "messages.h"
// #include "self_test.h"


#define DEBUG_FILE DBG_ESP_MAIN

enum class VEHICLE_STATE{
  SAFE_STATE, 
  IDLE, 
  MANNUAL,
  AUTONOMOUS, 
  DIAGNOSTICS, 
  FAIL_SAFE 
};

void run_vehicle_requested();
bool request_state_change(VEHICLE_STATE curent ,VEHICLE_STATE requested);

// system variables 
bool system_alive = true;
bool selftest_state = false;  
bool uart_connection_established = false; 
uint32_t diagnostics_WDT = 0; 

// set starting vehilce state. 
VEHICLE_STATE vehicle_state = VEHICLE_STATE::DIAGNOSTICS;               // set vehicle to safe state on boot.
VEHICLE_STATE return_state = VEHICLE_STATE::SAFE_STATE;                 // store the state to return to once diagnostics is complete.     

struct frame protocol_frame; 
uint8_t heartbeat = 0;

uint8_t tx_status = 0; 
uint8_t rx_status = 0; 


void setup(){
  
  // open debug port and print version data. 
  debug_port_begin();                                                    // open the debug port
  delay(1000);                                                           // run 1s delay before transmitting data 
  PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES);       // print version and meta data 

  // set the current transport & baud rate. 
  transport_set(&uart_io);                                               // set transport method default to be serial (&uart_io / &fifo_io)
  coms_port_begin(COMS_PORT_BAUD);                                       // start the port at baud rate defined in gloabl_config.h (default 115200)

}

void loop() {

  // update core systems 
  rx_status = update_rx_fsm();
  tx_status = update_tx_fsm();
  fifo_io_uart_engine_update();    // this only needs to run if transport diagnostics occours. 
  
  // run vehicle requested FSM. 
  run_vehicle_requested();
}


//*******************************************************************************************/
//*******************************************************************************************/


void run_vehicle_requested(){

  switch(vehicle_state){

    case VEHICLE_STATE::SAFE_STATE:

      //send out a request for confimation of receivig node
      if(uart_connection_established == false){
        //***** CHANGE THIS TO A DISPATCHER CALL *****//
        uint8_t t_data[] = {0,0,0,0,0,0};
        transport_queue_message(ESTABLISH_COMUNICATION, 1, 0, t_data);
        // if ack received update uart_connection_established = true; & update uart_wdt this can now be updated every 
        // time an ack frames is received, 
      }
      else if(uart_connection_established == true){
        //update_heartbeat(); 
        request_state_change(vehicle_state, VEHICLE_STATE::IDLE);
      } 

      // default start up requested untill confimed bi-direction comunication is established and hearbeat is started.
      // this is safe requested upon entry all control valused are set to a safe possiton
      // automatic resnc will staer if harbeat stoped
      // no commands are accepted or exceuted. 
      // i2c bus and sensor comunicaiton will stop.
      // any errors that caused the safe state to occour will be loged.
      break;

    case VEHICLE_STATE::IDLE:


    // comunication established and all busses live.
    // heartbeat monitored and upated. 
    // able to sellect: 
    //    running
    //    SW_update
    //    Diagnostic self tests. 
    //    retreive error & data logs
    break;

    case VEHICLE_STATE::MANNUAL: 


    // opperational commands accepted and vehicle driveable
    break;

    case VEHICLE_STATE::DIAGNOSTICS:  // FIXED TO this state at present. 




    selftest_state = run_test_case();

    if(selftest_state == true ){ // this line has an error changd from SELFTEST_ENDED to true as a temp solution. 
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "selftest completed.");
    }

    // WDT timer check for diagnostics runs for total test time ensure this id longer than the longest test of calibration. 
    if(millis() - diagnostics_WDT > DIAGNOSTIC_WT_TIMEOUT_MS){
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::DIAGNOSTICS Watchdog timer triggered.");
      vehicle_state = VEHICLE_STATE::FAIL_SAFE;
    }


    //self_tests and calibrations can be run in ths requested
    // heart beat is not monitored but checked & confirmed before switching back to pre running. 
    break;

    default:
      vehicle_state = VEHICLE_STATE::SAFE_STATE; 
    break; 
  };
}


//*******************************************************************************************/
//*******************************************************************************************/


bool request_state_change(VEHICLE_STATE curent,  VEHICLE_STATE requested){
  switch(curent){

    case VEHICLE_STATE::SAFE_STATE:
      if(requested == VEHICLE_STATE::IDLE && system_alive == true){
        vehicle_state = VEHICLE_STATE::IDLE;
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::SAFE_STATE;
        vehicle_state = VEHICLE_STATE::DIAGNOSTICS;
        diagnostics_WDT = millis();
        break;
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
      }
      else if(requested == VEHICLE_STATE::MANNUAL){
        vehicle_state = VEHICLE_STATE::MANNUAL;
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::IDLE; 
        vehicle_state = VEHICLE_STATE::DIAGNOSTICS; 
        diagnostics_WDT = millis();
      }
      else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::IDLE not avaliable form the current state");
        return false; 
      }
    return true;   
    break; 

      case VEHICLE_STATE::MANNUAL:
        if(requested == VEHICLE_STATE::SAFE_STATE){
          
          vehicle_state = VEHICLE_STATE::SAFE_STATE;
        }
        else if(requested == VEHICLE_STATE::IDLE){
          vehicle_state = VEHICLE_STATE::IDLE; 
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

      case VEHICLE_STATE::DIAGNOSTICS: 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: unable to request state transition form diagnostics.");
        return false; 
      break; 

      case VEHICLE_STATE::FAIL_SAFE:
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::FAIL_SAFE activated"); 
        return false; 
      break; 

    break;
  }
  return false; 
}
