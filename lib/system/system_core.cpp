
#include <Arduino.h>

#include "system.h"
#include "system_internal.h"

#include "self_test.h"
#include "logger.h"
#include "transport.h"
#include "protocol.h"
#include "simulation.h"

#include "debug.h"
#include "global_config.h"
#include "messages.h"

/*=============================================================================*
 * Debug Configuration
 *=============================================================================*/

#define DEBUG_FILE DBG_SYSTEM


/*=============================================================================*
 * Vehicle states 
 *=============================================================================*/

RX_RETURN_CODES rx_transport_status = RX_RETURN_CODES::UNINITIALIZED; 
TX_RETURN_CODES tx_transport_status = TX_RETURN_CODES::UNINITIALIZED;
uint8_t rx_protocol_status = 0;       // TODO conver this to an enum class. 

/*=============================================================================*
   * Core System Functions
 *=============================================================================*/

/*
 * Run all core system functions.
 *
 * This function is called continuously from the main application loop.
 * It is responsible for maintaining the execution order of the core
 * system modules.
 *
 * Module-specific state and processing remain inside their respective
 * modules. This function acts only as the system-level coordinator.
 */

void run_core_functions(){

  /*-------------------------------------------------------------------------*
    * Vehicle State Monitoring
  *-------------------------------------------------------------------------*
  * Detect vehicle state changes and report them to the debug system.
  * This information will later be used by the web interface.
  *-------------------------------------------------------------------------*/

  if(active_vehicle_state != last_vehicle_state){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "MAIN", "vehicle state cahnged: ", vehicle_state_to_string(active_vehicle_state));
    last_vehicle_state = active_vehicle_state; 
  }

  /*-------------------------------------------------------------------------*  
    * Transport Layer
  *-------------------------------------------------------------------------*/

  rx_transport_status = update_rx_fsm();
  tx_transport_status = update_tx_fsm();


  /*-------------------------------------------------------------------------*
    * Protocol Layer
  *-------------------------------------------------------------------------*/

  rx_protocol_status = rx_message_task_dispatcher();


  /*-------------------------------------------------------------------------*
    * Process Module Return Codes
  *-------------------------------------------------------------------------*/

  process_transport_rx_return_error(rx_transport_status);
  process_transport_tx_return_error(tx_transport_status);
  process_protocol_return_error(rx_protocol_status);


  /*-------------------------------------------------------------------------*
    * Simulation
  *-------------------------------------------------------------------------*/

  run_simulations();


}