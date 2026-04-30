
//////////////////////////////////////////////////////////////////////////////
//-------------------- BRANCH -self test implimnetation --------------------//
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <string.h>
#include "transport.h"
#include "transport_uart.h"
#include "transport_fifo.h"
#include "global_config.h"
//#include "debug.h"
//#include "debug_config.h"


// defines the name od the debug macro for this file
#define DEBUG_FILE DBG_TRANSPORT 

// macro to calculate the size of an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define CONVERT_US_TO_MS(us) ((us) / 1000)


#define UART_BITS_PER_BYTE 10
#define TX_BYTE_TRANSMISSION_TIME ((UART_BITS_PER_BYTE *1000000UL) / COMS_PORT_BAUD)

#define SIZE_OF_HEAD 5 //START + TYPE + ACK + ID + DLC 
#define SIZE_OF_TAIL 1 // CRC 


// ===== PRIVATE PROTOTYPES =====

static uint8_t inline_crc_calc(uint8_t CRC, uint8_t byte);
static bool rx_msg_wdt_check();
static void transmit_packet(struct frame *f);
static bool pack_message(uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, struct frame *ptr);
static void pack_ack(uint8_t type, uint8_t id, struct frame *f);
static void  copy_frame(struct frame *source, struct frame *destination);

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

 enum TX_STATE{
  TX_STATE_IDLE,
  TX_STATE_SENDING,
  TX_STATE_RETRY,
  TX_STATE_SUCCESS,
  TX_STATE_FAILED // handle errors here log files ect. 
 } tx_state; 

enum RX_STATE{
  RX_STATE_WAIT_START,
  RX_STATE_READ_TYPE,
  RX_STATE_READ_ACK,
  RX_STATE_READ_ID,
  RX_STATE_READ_DLC, 
  RX_STATE_READ_PAYLOAD, 
  RX_STATE_READ_CRC,
  RX_STATE_ERROR
  } rx_state; 

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

struct tx_packet{
  struct frame f;  
  bool waiting = false;
  uint8_t error_code = 0;
};

struct tx_ack_packet{
  struct frame f;
  bool waiting = false; 
  bool retry = false; 
  bool failed = false; 
  bool ack_confirmation = false; 
  uint32_t ack_timestamp = 0; 
  uint8_t retry_counter = 0; 
  uint8_t error_code = 0; 
};

struct rx_packet{
  struct frame f; 
  uint8_t crc_check = 0; 
  uint8_t payload_position = 0; 
  uint8_t error_code = 0; 
  bool ack_response = false; 
  bool frame_ready = false; 
};

struct rx_ack{
  uint8_t TYPE = 0x00; 
  uint8_t ID = 0x00;
  bool ack_valid = false; 

};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// structure to send a priority frame. 
struct tx_packet tx_priority_packet;
struct tx_packet tx_normal_packet; 
struct tx_ack_packet tx_pending_ack ;
struct rx_packet rx_packet; 
struct rx_ack rx_ack;

struct frame *active;                                  // active frame for sending data 
Transport_IO *current_transport = DEFAULT_TRANSPORT;   // communication port struct defined in transport.h and initialized in main.cpp

// global variables
uint32_t last_read_byte = 0x00; 
uint32_t tx_wdt_timestamp = 0; 
uint32_t rx_wdt_timestamp = 0; 
uint8_t packet_id = 0x01; 


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

bool transport_set(Transport_IO *io){
  if(io == NULL){
    //DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "PORT", "Invalid transport IO provided; current_transport: ", "uart_io");
    current_transport = DEFAULT_TRANSPORT;
    return false; 
  }
  current_transport = io;
  return true;
}

void fifo_io_uart_engine_update(){
   current_transport->update();
}

void coms_port_begin(uint32_t baud_rate){
  current_transport->begin(baud_rate);
}

static uint8_t inline_crc_calc(uint8_t CRC, uint8_t byte){
  CRC ^= byte;

  for (uint8_t i = 0; i < 8; i++) {
    if (CRC & 0x80){
     CRC = (CRC << 1) ^ 0x07;
    }
    else{
      CRC <<= 1;
    }
  }
  return CRC;
}



void flush_struct(struct frame *f){  // repalce tx / rx flush wth this make public!
  f->TYPE = 0x00;
  f->ACK = 0x00;
  f->ID = 0x00;
  f->DLC = 0x00;
  for(uint8_t i = 0; i < ARRAY_SIZE(f->payload); i ++ ){
    f->payload[i] = 0x00; 
  }
  f->CRC = 0x00;
}


void transport_pack_and_send_packet(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data){

  // send ack frame
  if(ack == ACK_RESPONSE){
    pack_ack(type, rx_packet.f.ID, &tx_priority_packet.f);
    tx_priority_packet.waiting = true;
    
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_ACK_MSG, "199", START_BYTE, "ACK_PAC", tx_priority_packet.f);
  }
  else if(pack_message(type, ack, packet_id, dlc, data, &tx_normal_packet.f) == true){
    tx_normal_packet.waiting = true;
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, "204", START_BYTE, "TX_PAC", tx_normal_packet.f);
  }
  // report an error with pack fuction
  else{
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "Failed to pack message. Message not sent.");
    tx_state = TX_STATE_FAILED;
    return;
  }
}

static bool pack_message(uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, struct frame *f){

  if(dlc > MAX_PAYLOAD_LEN){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "DLC", "DLC exceeds maximum payload length: ", dlc);
    return false; 
  }

  // flush the ack struct before packaging the frame.
  flush_struct(f); // flush the struct before packaging the frame  
  // pack frame with passed data.
  f->TYPE = type;
  f->ACK = ack;
  f->ID = id;
  f->DLC = dlc;
  for(uint8_t i = 0; i < f->DLC; i++){
    f->payload[i] = data[i];
  }
  f->CRC = 0; 

  // calculate CRC for data frame
  f->CRC = inline_crc_calc(f->CRC, f->TYPE);
  f->CRC = inline_crc_calc(f->CRC, f->ACK);
  f->CRC = inline_crc_calc(f->CRC, f->ID);
  f->CRC = inline_crc_calc(f->CRC, f->DLC);
  for(uint8_t i = 0; i < dlc; i++){ 
    f->CRC = inline_crc_calc(f->CRC, f->payload[i]);
  }  
  
  // debug print the packed frame before sendingg the frame.
  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, "244", START_BYTE, "PAC_PTR", f);
  return true;
}

static void pack_ack(uint8_t type, uint8_t id, struct frame *f){
  // flush the ack struct before packaging the frame.
  flush_struct(f); // flush the ack struct before packaging the frame. 

  //pack frame with passed data.
  f->TYPE = type;
  f->ACK = ACK_RESPONSE;
  f->ID = id;
  f->DLC = 0x00; // no data to sent in ACK.
  f->CRC = 0; 

  // calculate CRC for ACK frame 
  f->CRC = inline_crc_calc(f->CRC, f->TYPE);
  f->CRC = inline_crc_calc(f->CRC, f->ACK);
  f->CRC = inline_crc_calc(f->CRC, f->ID);
  f->CRC = inline_crc_calc(f->CRC, f->DLC);  
}

void transport_get_frame(struct frame *out){
  *out = rx_packet.f;
  DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MSG", "receive frame returned to protocal layer");
}


static bool rx_msg_wdt_check(){
  if(rx_state != RX_STATE_WAIT_START && (micros() - last_read_byte) > MSG_WDT_TIMEOUT_US){
    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "WDT", "over ", CONVERT_US_TO_MS(MSG_WDT_TIMEOUT_US), "ms");
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WDT", "watch dog timer timour occoured in rx_state: ", rx_state);
    return true;
  }
  else{
    return false; 
  }
}

static void transmit_packet(struct frame *f){
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

void copy_frame(struct frame *source, struct frame *destination){
  *destination = *source; 

  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_INFO, COPIED_FRAME, "318", START_BYTE, " copy from frame passed to copy. FROM: ", source);
  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_INFO, COPIED_FRAME, "319",START_BYTE, "copy to frame returned after copy opperation. TO:", destination);
  
  return;
}

///////////////// RECEIVE DATA FRAME ////////////////////

uint8_t read_data_frame(){
  
  uint8_t rx_return_status = RX_STATE_IDLE; //initiates the return value


  if (current_transport->available() > 0){
    
    uint8_t incoming = current_transport->read();
    last_read_byte = micros();

    switch(rx_state){

      case RX_STATE_WAIT_START: 

      if(incoming == START_BYTE){

        rx_packet.crc_check = 0;
        rx_packet.payload_position = 0;
        rx_packet.error_code = 0; 
        rx_packet.ack_response = false; 
        rx_packet.frame_ready = false; 

        rx_state = RX_STATE_READ_TYPE;
        rx_return_status = START_RECEIVED;

        DEBUG_PRINT_MSG_VAL_HEX(DEBUG_FILE, DEBUG_DEBUG, "RX", "start byte received: ", incoming);
        DEBUG_STREAM_START(DEBUG_FILE, DEBUG_STREAM, "RX", incoming);
      }
      else{
        rx_return_status = WAITING_FOR_START;
      }
      break;

      case RX_STATE_READ_TYPE:

        rx_packet.f.TYPE = incoming;
        rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming);

        rx_state = RX_STATE_READ_ACK;
        rx_return_status = TYPE_RECEIVED;

        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
      break;

      case RX_STATE_READ_ACK:

        rx_packet.f.ACK = incoming;
        rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming);

        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
    
        if(rx_packet.f.ACK == ACK_REQUEST){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK request received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = ACK_REQUEST_RECEIVED;
        }
        else if(rx_packet.f.ACK == ACK_RESPONSE){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK response received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = ACK_RESPONSE_RECEIVED;
        }
        else if(rx_packet.f.ACK == NACK){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "NACK request received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = NACK_REQUEST_RECEIVED;
        }
        else if(rx_packet.f.ACK == NORMAL_FRAME){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "normal frame received.");
          rx_state = RX_STATE_READ_ID;
          rx_return_status = NORMAL_FRAME_RECEIVED;
        }
        else{
          // handel ack out of range error 
          rx_packet.error_code = ACK_OUT_OF_RANGE;
          rx_return_status = ACK_OUT_OF_RANGE;
          rx_state = RX_STATE_ERROR;
        }
      break;

      case RX_STATE_READ_ID:
        rx_packet.f.ID = incoming;
        rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming);
        
        rx_state = RX_STATE_READ_DLC;
        rx_return_status = ID_RECEIVED; 

        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
      break;
      
      case RX_STATE_READ_DLC: 
        if(incoming >= ARRAY_SIZE(rx_packet.f.payload)){
          
          rx_return_status = DLC_OVER_CAPACITY;
          rx_packet.error_code = DLC_OVER_CAPACITY;
          rx_state = RX_STATE_ERROR;
        }
        else{
          rx_packet.f.DLC = incoming;
          rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming);
          rx_state = RX_STATE_READ_PAYLOAD;
          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
          rx_return_status = DLC_RECEIVED;
        }
      break; 

      case RX_STATE_READ_PAYLOAD:
        if(rx_packet.payload_position < rx_packet.f.DLC){
          rx_packet.f.payload[rx_packet.payload_position] = incoming;
          rx_packet.crc_check = inline_crc_calc(rx_packet.crc_check, incoming);
          rx_packet.payload_position ++; 
          
          rx_return_status = RECEIVING_DATA;

          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
        }
        if (rx_packet.payload_position == rx_packet.f.DLC){
          rx_state = RX_STATE_READ_CRC;
          rx_return_status = PAYLOAD_COMPLETE;
        }
        else if (rx_packet.payload_position > rx_packet.f.DLC){
          // error detected
          rx_state = RX_STATE_ERROR;
          rx_return_status = PAYLOAD_OVERFLOW; 
          rx_packet.error_code = PAYLOAD_OVERFLOW;
        }
      break;

      case RX_STATE_READ_CRC:

        rx_packet.f.CRC = incoming;

        // check and report corupt frames
        if(rx_packet.crc_check != rx_packet.f.CRC){

          rx_state = RX_STATE_ERROR;
          rx_return_status = CRC_ERROR; 
          rx_packet.error_code = CRC_ERROR;

          DEBUG_STREAM_END(DEBUG_FILE, DEBUG_STREAM, incoming);
        
        }
        else{ 
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "CRC", "CRC check OK.");

          if(rx_packet.f.ACK == ACK_REQUEST){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "RX", "responding to ACK request.");
            pack_ack(rx_packet.f.TYPE, rx_packet.f.ID, &tx_priority_packet.f);
            tx_priority_packet.waiting = true;
          }
          else if(rx_packet.f.ACK == ACK_RESPONSE){ 
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "ACK", "received ack response. ");
            rx_ack.TYPE = rx_packet.f.TYPE;
            rx_ack.ID = rx_packet.f.ID;
            rx_ack.ack_valid = true;
            rx_return_status = ACK_READY;
          } 
          else{
            rx_packet.frame_ready = true; 
            rx_return_status = FRAME_READY; 
          }

          rx_state = RX_STATE_WAIT_START;

          DEBUG_STREAM_END(DEBUG_FILE, DEBUG_STREAM, incoming);
          DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, RX_FRAME, "462", START_BYTE, "[RX]", rx_packet.f);
        }  
      break;
    

      case RX_STATE_ERROR: 
        if(rx_packet.error_code == INVALID_TYPE)             { /*report type error here*/ }
        else if(rx_packet.error_code == ACK_OUT_OF_RANGE )   {DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "ACK", "out of range ACK: ", rx_packet.f.ACK);}
        else if(rx_packet.error_code == DLC_OVER_CAPACITY)   {DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "RX", "DLC greater that payload capacity: ", ARRAY_SIZE(rx_packet.f.payload));}
        else if(rx_packet.error_code == PAYLOAD_OVERFLOW)    {DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "DLC", "Payload exceded DLC: ", rx_packet.payload_position);}
        else if(rx_packet.error_code == MSG_TIMEOUT_ERROR)   {DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "RX", "RX watch dog timer trigered elapsed time: ", CONVERT_US_TO_MS((rx_wdt_timestamp - last_read_byte)), "ms");}
        else if(rx_packet.error_code == CRC_ERROR)           {DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "CRC", "CRC check NOK. calculated CRC: ", rx_packet.crc_check);}

      break;
    }
  }
  // cehcek the message WTD for timeout error rest only if the state is idle nad rx is in progress
  if(rx_msg_wdt_check() == true && rx_state != RX_STATE_WAIT_START && rx_return_status == RX_STATE_IDLE ){
    rx_wdt_timestamp = micros();
    rx_state = RX_STATE_ERROR;
    rx_return_status =  MSG_TIMEOUT_ERROR; 
    rx_packet.error_code = MSG_TIMEOUT_ERROR;
  }

  return rx_return_status;
}


///////////////// SEND DATA FRAME ////////////////////
 
uint8_t send_data_frame(){

  uint8_t tx_return_status = 0; 
  // priorities the message based on the type. (ACK, RESEND, NORMAL)
  if((tx_state == TX_STATE_IDLE) && (tx_priority_packet.waiting == true)){
    active = &tx_priority_packet.f;
    tx_priority_packet.waiting = false; 
    tx_state = TX_STATE_SENDING;
  }
  else if((tx_state == TX_STATE_IDLE) && (tx_pending_ack.retry == true)){
    active = &tx_pending_ack.f;
    tx_state = TX_STATE_RETRY;
    tx_pending_ack.retry = false; 
  }
  else if((tx_state == TX_STATE_IDLE) && tx_normal_packet.waiting == true){
      active = &tx_normal_packet.f;
      tx_normal_packet.waiting = false; 
      tx_state = TX_STATE_SENDING;  
  }

  switch(tx_state){

    case TX_STATE_IDLE:
      tx_return_status = TX_IDLE_STATE; 
    break;
    

    case TX_STATE_SENDING:{ 
        //transmit data packet rx_packet is not currently used
        if(current_transport->available() < (MAX_SERIAL_BUFFER_SIZE - sizeof(tx_normal_packet.f))){
          transmit_packet(active);
          DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_MSG, TX_FRAME, "533", START_BYTE, "TX_PTR", active);
        }
        else{
          DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "tx serail buffer full. peding: ", (MAX_SERIAL_BUFFER_SIZE - sizeof(tx_normal_packet.f)), " bytes");
          tx_state = TX_STATE_FAILED;
          tx_return_status = TX_BUFFER_OVERFLOW;

        }

        if(active->ACK == ACK_REQUEST){
          copy_frame(active, &tx_pending_ack.f);

          // reset the all parameters to default 
          tx_pending_ack.ack_confirmation = false; 
          tx_pending_ack.waiting = true;
          tx_pending_ack.retry_counter = 0; 
          tx_pending_ack.ack_timestamp = micros();
          
          tx_state = TX_STATE_SUCCESS;
          tx_return_status = TX_PENDING_ACK;
        }
        else{
          tx_state = TX_STATE_SUCCESS;
          tx_return_status = TX_TRANSMITING;
        }
      break; 
    }


    case TX_STATE_RETRY:{ // modify this to only resend the packt and increment the retrie counter 

        if(tx_pending_ack.retry_counter < TX_MAX_RETRIES){

          transmit_packet(&tx_pending_ack.f);
          tx_pending_ack.retry_counter ++; 
          tx_pending_ack.ack_timestamp = micros();

          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "TX", " ACK not receeived atteming to resend message. retry: ", tx_pending_ack .retry_counter);
          
          tx_state = TX_STATE_IDLE;
          tx_return_status = RESENDING_MSG;
        }
        else{
          // report ack not received
          tx_pending_ack .failed = true;

          tx_pending_ack .error_code = ACK_NOT_RECEVIED; 
          tx_state = TX_STATE_FAILED;
          tx_return_status = TX_ERROR;
        }
      break;
    }

    case TX_STATE_SUCCESS:{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TX", "TX transmission successful.");

        packet_id ++; // only incremt on sucsesfull transmition never set to 0. 
        if(packet_id == 0){
          packet_id = 1; 
        }

        tx_state = TX_STATE_IDLE; // this is line 527
        tx_return_status = TX_SUCCESS;
      break;
    }

    case TX_STATE_FAILED:{
        // report ack errors 
        if(tx_pending_ack.error_code == ACK_NOT_RECEVIED){
          DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "No ACK receeived after ", tx_pending_ack .retry_counter, " attemps.");
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "packet failed to send");
        }
        else if(tx_pending_ack.error_code == ACK_MISMATCHED){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TX", "ACK mismatched received ack ID: ", rx_packet.f.ID);
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TX", "ACK mismatched received TYPE: ", rx_packet.f.TYPE);
        }
        else if(tx_pending_ack.error_code == TX_BUFFER_OVERFLOW || tx_priority_packet.error_code == TX_BUFFER_OVERFLOW || tx_normal_packet.error_code == TX_BUFFER_OVERFLOW){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "TX", "failed to send packet due to serial bufer overflow. ");
        }

        tx_state = TX_STATE_IDLE;
        tx_return_status = TX_ERROR;
      break;  
    }
  }

  //check_tx_pending_ack _WDT
  if(tx_pending_ack.waiting == true && tx_pending_ack.failed == false && (micros() - tx_pending_ack.ack_timestamp) > ACK_WDT_TIMEOUT_US){
    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "ACK", "ack_watchdog timer overflowed. elapsed time:", (micros() - tx_pending_ack.ack_timestamp), "us");
    tx_state = TX_STATE_RETRY;
  }

  // check for ack confirmation here!
  if(rx_packet.ack_response == true){ 
    if(rx_packet.f.TYPE == rx_ack.TYPE && rx_ack.ID == tx_pending_ack.f.ID){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TX", "ack received cheked OK.");
    
        tx_pending_ack.waiting  = false;

        rx_ack.TYPE = 0x00;
        rx_ack.ID = 0x00;
        rx_ack.ack_valid = false;
    }
    else{ 
      rx_ack.TYPE = 0x00;
      rx_ack.ID = 0x00;
      rx_ack.ack_valid = false;

      tx_state = TX_STATE_FAILED;
      tx_return_status = ACK_MISMATCHED;
      tx_pending_ack.error_code = ACK_MISMATCHED; 
    }
  }

  return tx_return_status;
}
