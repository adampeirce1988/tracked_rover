
#include "transport_internal.h"
#include "transport_structs.h"
#include "debug.h"

/*=============================================================================
    Debug Configuration
=============================================================================*/

#define DEBUG_FILE DBG_TRANSPORT
 

/*=============================================================================
    Runtime Objects
=============================================================================*/

struct transmit_packet      tx_priority_packet;
struct transmit_packet      tx_normal_packet;
struct transmit_ack_packet  tx_pending_ack;

struct receive_packet       rx_packet;
struct receive_ack          rx_ack;

struct frame *active = nullptr;
                               

/*=============================================================================
    Transport Interface
=============================================================================*/

bool transport_set(Transport_IO *io){
  if(io == nullptr){
    current_transport = DEFAULT_TRANSPORT;
    return false; 
  }
  current_transport = io;
  return true;
}


/*=============================================================================
    CRC Functions
=============================================================================*/

 uint8_t inline_crc_calc(uint8_t crc, uint8_t byte){
  crc ^= byte;

  for (uint8_t i = 0; i < 8; i++) {
    if (crc & 0x80){
     crc = (crc << 1) ^ 0x07;
    }
    else{
      crc <<= 1;
    }
  }
  return crc;
}


 uint8_t calculate_crc(struct frame *f){

  uint8_t crc = 0; 

  crc = inline_crc_calc(crc, f->TYPE);
  crc = inline_crc_calc(crc, f->ACK);
  crc = inline_crc_calc(crc, f->ID);
  crc = inline_crc_calc(crc, f->DLC);

  if(f->DLC > 0){
    for(uint8_t i = 0; i < f->DLC; i++){
      crc = inline_crc_calc(crc, f->payload[i]);
    }
  }
  return crc;
}


/*=============================================================================
    Frame Utilities
=============================================================================*/

 void flush_struct(struct frame *f){ 
  f->TYPE = 0x00;
  f->ACK = 0x00;
  f->ID = 0x00;
  f->DLC = 0x00;
  for(uint8_t i = 0; i < ARRAY_SIZE(f->payload); i ++ ){
    f->payload[i] = 0x00; 
  }
  f->CRC = 0x00;
}


/*=============================================================================
    Packet Construction
=============================================================================*/

void transport_queue_message(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data){
  // send ack frame
  if(ack == TRANSPORT_ACK_TYPE::ACK_RESPONSE){
    pack_ack(type, rx_packet.f.ID, &tx_priority_packet.f);
    tx_priority_packet.waiting = true;
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_ACK_MSG, "199", START_BYTE, "ACK_PAC", tx_priority_packet.f);
  }
  else if(pack_message(type, ack, packet_id, dlc, data, &tx_normal_packet.f) == true){
    tx_normal_packet.waiting = true;
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, "204", START_BYTE, "TX_PAC", tx_normal_packet.f);
  }
  // report an error with pack function
  else{
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "Failed to pack message. Message not sent.");
    tx_state = TX_STATE_FAILED;
    return;
  }
}

bool pack_message(uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, struct frame *f){

  if(dlc > MAX_PAYLOAD_LEN){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "DLC", "DLC exceeds maximum payload length: ", dlc);
    return false; 
  }

  // Flush the ACK frame before packaging.
  flush_struct(f); 

  // Populate the ACK frame.
  f->TYPE = type;
  f->ACK = ack;
  f->ID = id;
  f->DLC = dlc;
  if(data != nullptr){ // Check for no data condition.
    for(uint8_t i = 0; i < f->DLC; i++){  
      f->payload[i] = data[i];
    }
  }  
  f->CRC = calculate_crc(f); 

  // Apply frame error injection if tx_fault_injection_cfg.tx_fault_injection_flag == true
  tx_frame_error_injection(f);

  // Debug-print the packed frame before transmission.
  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, "244", START_BYTE, "PAC_PTR", f);
  return true;
}


void pack_ack(uint8_t type, uint8_t id, struct frame *f){
  
  // Flush the ACK frame before packaging.
  flush_struct(f); 

  // Populate the ACK frame.
  f->TYPE = type;
  f->ACK = TRANSPORT_ACK_TYPE::ACK_RESPONSE;
  f->ID = id;
  f->DLC = 0x00; // no data to send in ACK.
  f->CRC = calculate_crc(f); 
}


/*=============================================================================
    Receive Utilities
=============================================================================*/

void transport_get_frame(struct frame *out){
  *out = rx_packet.f;
  rx_packet.frame_ready = false; 
  DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MSG", "Received frame returned to protocol layer.");
}


bool rx_msg_wdt_check(){
  if(rx_state != RX_STATE_WAIT_START && (micros() - last_read_byte) > MSG_WDT_TIMEOUT_US){
    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "WDT", "rx incoming data watchdog timer timeout. elapsed time: ", (micros() - last_read_byte), "us");
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WDT", "Watchdog timer timeout occurred in rx_state: ", rx_state);
    return true;
  }
  
  return false; 
}


void reset_rx_message_struct(){
  // flush the rx_packet struct
  flush_struct(&rx_packet.f); 

  // reset the rx_packet parameters
  rx_packet.crc_check = 0; 
  rx_packet.error_code = RX_RETURN_CODES::UNINITIALIZED; 
  rx_packet.frame_ready = false; 
  rx_packet.payload_position = 0; 
}


void clear_rx_ack(){
  rx_ack.TYPE = 0x00;
  rx_ack.ID = 0x00;
  rx_ack.ack_valid = false;
}


/*=============================================================================
    Transmission
=============================================================================*/

void transmit_packet(struct frame *f){
  current_transport->write(START_BYTE); 
  current_transport->write(f->TYPE);
  current_transport->write(f->ACK);
  current_transport->write(f->ID);
  current_transport->write(f->DLC);
  for(uint8_t i = 0; i < f->DLC; i++){
    current_transport->write(f->payload[i]);
  }
  current_transport->write(f->CRC);
}


void copy_frame(struct frame *src, struct frame *dst){
  *dst = *src; 

  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_INFO, COPIED_FRAME, "318", START_BYTE, " copy from frame passed to copy operation. FROM: ", src);
  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_INFO, COPIED_FRAME, "319",START_BYTE, "copy to frame returned after copy operation. TO:", dst);
  
}


/*=============================================================================
    Diagnostics
=============================================================================*/

uint16_t transport_get_tx_latency(){  
  if(total_tx_frame_time > UINT16_MAX){
    return UINT16_MAX; 
  }

  return total_tx_frame_time; 
}


uint16_t transport_get_rx_latency(){
  if(total_rx_frame_time > UINT16_MAX){
    return UINT16_MAX; 
  }

  return total_rx_frame_time;  
}
