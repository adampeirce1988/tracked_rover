
// TODO: on return to safe_state the paramer sys::comunication esatbolished to false on error. 

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

#define DEBUG_FILE DBG_ESP_MAIN

// return code variable ** these are also stored in system.h** 
uint8_t rx_transport_status = 0;  
uint8_t tx_transport_status = 0;
uint8_t rx_protocol_status = 0; 
SELF_TEST_MANAGER_RETURN_CODE self_testmanager_status = SELF_TEST_MANAGER_RETURN_CODE::ERROR;

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
  
  // print state change only on change. - to be dispalyed in the web interface on rlease
  if(active_vehicle_state != last_vehicle_state){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "MAIN", "vehicle state cahnged active_vehicle_state: ", vehicle_state_to_string(active_vehicle_state));
    last_vehicle_state = active_vehicle_state; 
  }

  rx_transport_status = update_rx_fsm(); 
  tx_transport_status = update_tx_fsm();

  // check for avaliable packets and run the dispacher - [TODO]: add a bit wise check for valid type 
  if(frame_avaliable() > 0){
    rx_protocol_status = rx_message_task_dispatcher(); 
  }

  // process returned codes & log if an error is received
  process_transport_rx_return_error(rx_transport_status); 
  process_transport_tx_return_error(tx_transport_status); 
  process_protocol_return_error(rx_protocol_status); 
      
  fifo_io_uart_engine_update();                                  // this only needs to run if transport diagnostics occours. 
  
  run_vehicle_state();                                           // run vehicle requested FSM. 
}
