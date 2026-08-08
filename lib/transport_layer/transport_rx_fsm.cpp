#include "transport_internal.h"
#include "debug.h"


/*=============================================================================
    Debug Configuration
=============================================================================*/

#define DEBUG_FILE DBG_TRANSPORT


/*=============================================================================
    Runtime Objects
=============================================================================*/

uint32_t last_read_byte        = 0x00; 
uint32_t rx_timeout_timestamp  = 0; 
uint32_t rx_start_timestamp    = 0;
uint32_t total_rx_frame_time   = 0;

RX_STATE rx_state = RX_STATE_WAIT_START; 


/*=============================================================================
    Receive State Machine
=============================================================================*/

RX_RETURN_CODES update_rx_fsm()
{
  
  RX_RETURN_CODES rx_return_status = RX_RETURN_CODES::UNINITIALIZED;
  
  uint16_t available_bytes = current_transport->available();  // get the current amount of data
  uint8_t incoming_byte = 0;                                      // store the read byte of data 

  /*-------------------------------------------------------------------------
     Read Incoming Data
  -------------------------------------------------------------------------*/

  if(available_bytes > 0 || rx_state == RX_STATE_ERROR){
    
    // only read bytes if data available.  
    if(available_bytes > 0 && rx_state != RX_STATE_ERROR){
      incoming_byte = current_transport->read();
      last_read_byte = micros();
    }


    /*---------------------------------------------------------------------
        RX State Machine
    ---------------------------------------------------------------------*/

    switch(rx_state)
    {

      case RX_STATE_WAIT_START: 
        if(incoming_byte == START_BYTE){
          
          rx_start_timestamp = micros();

          reset_rx_message_struct();

          rx_state = RX_STATE_READ_TYPE;
          rx_return_status = RX_RETURN_CODES::START_RECEIVED;

          DEBUG_PRINT_MSG_VAL_HEX(DEBUG_FILE, DEBUG_INFO, "RX", "start byte received: ", incoming_byte);
          DEBUG_STREAM_START(DEBUG_FILE, DEBUG_STREAM, "RX", incoming_byte);
        }
        else{
          rx_return_status = RX_RETURN_CODES::WAITING_FOR_START;
        }
      break;

      case RX_STATE_READ_TYPE:

        rx_packet.f.TYPE = incoming_byte;
        rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming_byte);

        rx_state = RX_STATE_READ_ACK;
        rx_return_status = RX_RETURN_CODES::TYPE_RECEIVED;

        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming_byte);
      break;

      case RX_STATE_READ_ACK:

        rx_packet.f.ACK = incoming_byte;
        rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming_byte);

        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming_byte);
    
        if(rx_packet.f.ACK == TRANSPORT_ACK_TYPE::ACK_REQUEST){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK request received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = RX_RETURN_CODES::ACK_REQUEST_RECEIVED;
        }
        else if(rx_packet.f.ACK == TRANSPORT_ACK_TYPE::ACK_RESPONSE){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK response received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = RX_RETURN_CODES::ACK_RESPONSE_RECEIVED;
        }
        else if(rx_packet.f.ACK == TRANSPORT_ACK_TYPE::NACK){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "NACK request received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = RX_RETURN_CODES::NACK_REQUEST_RECEIVED;
        }
        else if(rx_packet.f.ACK == TRANSPORT_ACK_TYPE::NORMAL_FRAME){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "normal frame received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = RX_RETURN_CODES::NORMAL_FRAME_RECEIVED;
        }
        else{
          // handle ack out of range error 
          rx_packet.error_code = RX_RETURN_CODES::ACK_OUT_OF_RANGE;
          rx_return_status = RX_RETURN_CODES::ACK_OUT_OF_RANGE;
          rx_state = RX_STATE_ERROR;
        }
      break;

      case RX_STATE_READ_ID:
        rx_packet.f.ID = incoming_byte;
        rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming_byte);
        
        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming_byte);

        rx_state = RX_STATE_READ_DLC;
        rx_return_status = RX_RETURN_CODES::ID_RECEIVED; 
      break;
      
      case RX_STATE_READ_DLC: 
        if(incoming_byte > MAX_PAYLOAD_LEN){
          rx_return_status = RX_RETURN_CODES::DLC_OVER_CAPACITY;
          rx_packet.error_code = RX_RETURN_CODES::DLC_OVER_CAPACITY;
          rx_state = RX_STATE_ERROR;
        } 
        else{
          rx_packet.f.DLC = incoming_byte;
          rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming_byte);

          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming_byte);

          if(rx_packet.f.DLC == 0){
            rx_state = RX_STATE_READ_CRC;
            rx_return_status = RX_RETURN_CODES::DLC_RECEIVED;
          }
          else{
            rx_state = RX_STATE_READ_PAYLOAD;
            rx_return_status = RX_RETURN_CODES::DLC_RECEIVED;
          }
        }
      break; 

      case RX_STATE_READ_PAYLOAD:
        if(rx_packet.payload_position < rx_packet.f.DLC){
          rx_packet.f.payload[rx_packet.payload_position] = incoming_byte;
          rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming_byte);
          rx_packet.payload_position ++; 
          
          rx_return_status = RX_RETURN_CODES::RECEIVING_DATA;

          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming_byte);
        }
        if (rx_packet.payload_position == rx_packet.f.DLC){
          rx_state = RX_STATE_READ_CRC;
          rx_return_status = RX_RETURN_CODES::PAYLOAD_COMPLETE;
        }
        else if (rx_packet.payload_position > rx_packet.f.DLC){
          rx_state = RX_STATE_ERROR;
          rx_return_status = RX_RETURN_CODES::PAYLOAD_OVERFLOW; 
          rx_packet.error_code = RX_RETURN_CODES::PAYLOAD_OVERFLOW;
        }
      break;

      case RX_STATE_READ_CRC:

        rx_packet.f.CRC = incoming_byte;
        
        
        DEBUG_STREAM_END(DEBUG_FILE, DEBUG_STREAM, incoming_byte);
        DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, RX_FRAME, "", START_BYTE, "[RX]", rx_packet.f);

        // Validate CRC
        if(rx_packet.crc_check != rx_packet.f.CRC){
          rx_return_status = RX_RETURN_CODES::CRC_ERROR; 
          rx_packet.error_code = RX_RETURN_CODES::CRC_ERROR;
          rx_state = RX_STATE_ERROR;
          break; 
        }
        else{ 
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "CRC", "CRC check OK.");
          
          if(rx_packet.f.ACK == TRANSPORT_ACK_TYPE::ACK_REQUEST){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "RX", "responding to ACK request.");
            pack_ack(rx_packet.f.TYPE, rx_packet.f.ID, &tx_priority_packet.f);
            tx_priority_packet.waiting = true;
            rx_packet.frame_ready = true;
            rx_return_status = RX_RETURN_CODES::FRAME_READY;
          }
          else if(rx_packet.f.ACK == TRANSPORT_ACK_TYPE::ACK_RESPONSE){ 
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "received ack response. ");
            rx_ack.TYPE = rx_packet.f.TYPE;
            rx_ack.ID = rx_packet.f.ID;
            rx_ack.ack_valid = true;
            rx_return_status = RX_RETURN_CODES::ACK_READY;
          } 
          else{
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "NORM", "normal packet received ");
            rx_packet.frame_ready = true; 
            rx_return_status = RX_RETURN_CODES::FRAME_READY; 
          }

          // Record total RX frame processing time. 
          total_rx_frame_time = micros() - rx_start_timestamp;

          rx_state = RX_STATE_WAIT_START;
        }  
      break;
    

      case RX_STATE_ERROR: 
        if(rx_packet.error_code == RX_RETURN_CODES::INVALID_TYPE){
           /*report type error here*/
        }
        else if(rx_packet.error_code == RX_RETURN_CODES::ACK_OUT_OF_RANGE ){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "ACK", "out of range ACK: ", static_cast<uint8_t>(rx_packet.f.ACK));
          
        }
        else if(rx_packet.error_code == RX_RETURN_CODES::DLC_OVER_CAPACITY){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "RX", "DLC greater than payload capacity: ", ARRAY_SIZE(rx_packet.f.payload));
          
        }
        else if(rx_packet.error_code == RX_RETURN_CODES::PAYLOAD_OVERFLOW){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "DLC", "Payload exceeded DLC: ", rx_packet.payload_position);
          
        }
        else if(rx_packet.error_code == RX_RETURN_CODES::MSG_TIMEOUT_ERROR){
          DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "RX", "RX watchdog timer triggered elapsed time: ", CONVERT_US_TO_MS((rx_timeout_timestamp - last_read_byte)), "ms");
          
        }
        else if(rx_packet.error_code == RX_RETURN_CODES::CRC_ERROR){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "CRC", "Received CRC: ", rx_packet.f.CRC);
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "CRC", "Calculated CRC: ", rx_packet.crc_check);

        }
        
        // Reset RX context after error.
        reset_rx_message_struct();

        rx_state = RX_STATE_WAIT_START;
      break;
    }
  }


  /*-------------------------------------------------------------------------
    RX Watchdog
  -------------------------------------------------------------------------*/

  // Ignore timeout when waiting for a new frame.
  if(rx_msg_wdt_check() == true && rx_state != RX_STATE_WAIT_START){
    rx_timeout_timestamp = micros();
    rx_return_status =  RX_RETURN_CODES::MSG_TIMEOUT_ERROR; 
    rx_packet.error_code = RX_RETURN_CODES::MSG_TIMEOUT_ERROR;
    rx_state = RX_STATE_ERROR;
  }

  return rx_return_status;
}

