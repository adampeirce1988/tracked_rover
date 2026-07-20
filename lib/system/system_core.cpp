#include <Arduino.h>
#include <HardwareSerial.h>
#include "self_test.h"
#include "logger.h"
#include "transport.h"
#include "debug.h"
#include "protocol.h"
#include "global_config.h"
#include "global.h"
#include "messages.h"
#include "system.h"
#include "simulation.h"

#define DEBUG_FILE DBG_SYSTEM


void run_core_functions(){

    // print state change only on change. - to be dispalyed in the web interface on rlease
  if(active_vehicle_state != last_vehicle_state){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "MAIN", "vehicle state cahnged active_vehicle_state: ", vehicle_state_to_string(active_vehicle_state));
    last_vehicle_state = active_vehicle_state; 
  }

  rx_transport_status = update_rx_fsm(); 
  tx_transport_status = update_tx_fsm();

  // check for avaliable packets and run the dispacher
  if(frame_avaliable() > 0){
    rx_protocol_status = rx_message_task_dispatcher(); 
  }

  // process returned codes & log if an error is received
  process_transport_rx_return_error(rx_transport_status); 
  process_transport_tx_return_error(tx_transport_status); 
  process_protocol_return_error(rx_protocol_status); 
  
  // fun simulation engines
  run_simulations();  

}