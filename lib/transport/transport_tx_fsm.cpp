
#include "global.h"
#include "transport.h"
#include "transport_internal.h"
#include "debug.h"

/*=============================================================================
    Debug Configuration
=============================================================================*/

#define DEBUG_FILE DBG_TRANSPORT


/*=============================================================================
    Runtime Objects
=============================================================================*/

TX_STATE tx_state; 

uint32_t tx_start_timestamp   = 0; 
uint32_t total_tx_frame_time  = 0;

uint8_t packet_id             = 0x01; 


/*=============================================================================
    Transmit State Machine
=============================================================================*/
 
TX_RETURN_CODES update_tx_fsm(){

  TX_RETURN_CODES tx_return_status = TX_RETURN_CODES::UNINITIALIZED;  

  /*---------------------------------------------------------------------
      Message Selection
  ---------------------------------------------------------------------*/

  // Select the highest priority frame pending transmission.

  if((tx_state == TX_STATE_IDLE) && (tx_priority_packet.waiting == true)){
    active = &tx_priority_packet.f;
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PRIORITY, "", START_BYTE, "PRIORITY", tx_priority_packet.f);
    tx_priority_packet.waiting = false; 
    tx_state = TX_STATE_SENDING;
    tx_return_status = TX_RETURN_CODES::TX_ACK_TRANSMISSION_SUCCESS;  
  }
  else if((tx_state == TX_STATE_IDLE) && (tx_pending_ack.retry == true)){
    active = &tx_pending_ack.f;
    tx_state = TX_STATE_RETRY;
    tx_pending_ack.retry = false; 
    tx_return_status = TX_RETURN_CODES::TX_RETRY_TRANSMISSION_SUCCESS;
  }
  else if((tx_state == TX_STATE_IDLE) && tx_normal_packet.waiting == true){
    active = &tx_normal_packet.f;
    tx_normal_packet.waiting = false; 
    tx_state = TX_STATE_SENDING; 
    tx_return_status = TX_RETURN_CODES::TX_TRANSMISSION_SUCCESS; 
  }


  /*---------------------------------------------------------------------
      Transmit State Machine
  ---------------------------------------------------------------------*/

  switch(tx_state){

    case TX_STATE_IDLE:
      tx_return_status = TX_RETURN_CODES::TX_IDLE_STATE; 
    break;
    

    case TX_STATE_SENDING:{ 

        // Record transmission start time 
        tx_start_timestamp = micros();

        // Transmit the selected frame.
        transmit_packet(active);
        DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_MSG, TX_FRAME, "", START_BYTE, "TX_PTR", active);

        if(active->ACK == TRANSPORT_ACK_TYPE::ACK_REQUEST){
          copy_frame(active, &tx_pending_ack.f);

          // Initialise ACK tracking state. 
          tx_pending_ack.ack_confirmation = false; 
          tx_pending_ack.waiting = true;
          tx_pending_ack.retry_counter = 0; 
          tx_pending_ack.ack_timestamp = micros();
        }
      
          tx_state = TX_STATE_SUCCESS;                          
     
      break; 
    }


    case TX_STATE_RETRY:{

      if(tx_pending_ack.retry_counter < TX_MAX_RETRIES){

        transmit_packet(&tx_pending_ack.f);
        tx_pending_ack.retry_counter ++; 
        tx_pending_ack.ack_timestamp = micros();

        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TX", " ACK not received attempting to resend message. retry: ", tx_pending_ack.retry_counter);
        
        tx_state = TX_STATE_IDLE;
        tx_return_status = TX_RETURN_CODES::RESENDING_MSG;        
      }
      else{
        // Maximum retry count exceeded.
        tx_pending_ack.failed = true;

        tx_pending_ack.error_code = TX_RETURN_CODES::ACK_NOT_RECEIVED; 
        tx_state = TX_STATE_FAILED;
        tx_return_status = TX_RETURN_CODES::TX_TRANSMISSION_ERROR;

      }
      break;
    }

    case TX_STATE_SUCCESS:{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TX", "TX transmission successful.");

        packet_id++;

        tx_state = TX_STATE_IDLE;

        // Record the transmission duration.
        // TODO: Record longest and shortest transmission times in the runtime metrics.
        total_tx_frame_time = micros() - tx_start_timestamp;

      break;
    }

    case TX_STATE_FAILED:{
        // Report transmission failure.
        if(tx_pending_ack.error_code == TX_RETURN_CODES::ACK_NOT_RECEIVED){
          DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "No ACK received after ", tx_pending_ack .retry_counter, " attempts.");
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "packet failed to send");
        }
        else if(tx_pending_ack.error_code == TX_RETURN_CODES::ACK_MISMATCHED){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TX", "ACK mismatched received ack ID: ", rx_packet.f.ID);
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TX", "ACK mismatched received TYPE: ", rx_packet.f.TYPE);
        }
        else if(
          tx_pending_ack.error_code == TX_RETURN_CODES::TX_BUFFER_OVERFLOW ||
          tx_priority_packet.error_code == TX_RETURN_CODES::TX_BUFFER_OVERFLOW || 
          tx_normal_packet.error_code == TX_RETURN_CODES::TX_BUFFER_OVERFLOW)
        {
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "failed to send packet due to serial buffer overflow. ");
        }
        tx_state = TX_STATE_IDLE;
        tx_return_status = TX_RETURN_CODES::TX_TRANSMISSION_ERROR;
      break;  
    }
  }

   
  /*---------------------------------------------------------------------
      ACK Watchdog
  ---------------------------------------------------------------------*/

  // Monitor ACK timeout.
  if(tx_pending_ack.waiting == true && tx_pending_ack.failed == false && (micros() - tx_pending_ack.ack_timestamp) > ACK_WDT_TIMEOUT_US){
    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "ACK", "ack_watchdog timer overflowed. elapsed time:", (micros() - tx_pending_ack.ack_timestamp), "us");
    tx_state = TX_STATE_RETRY;
  }

  /*---------------------------------------------------------------------
    ACK Validation
  ---------------------------------------------------------------------*/

  // Validate received ACK
  if(rx_ack.ack_valid == true){ 
    if(tx_pending_ack.f.TYPE == rx_ack.TYPE && tx_pending_ack.f.ID == rx_ack.ID){
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TX", "ACK validation passed.");
    
      tx_pending_ack.waiting = false;
        
      // update system watchdog for the last valid comms.
      update_last_valid_comms();

      // clear the rx_ack parameters
      clear_rx_ack(); 

    }
    else{ 
      // clear the rx_ack parameters
      clear_rx_ack();

      tx_state = TX_STATE_FAILED;
      tx_return_status = TX_RETURN_CODES::ACK_MISMATCHED;
      tx_pending_ack.error_code = TX_RETURN_CODES::ACK_MISMATCHED; 
    }
  }

  return tx_return_status;
}
