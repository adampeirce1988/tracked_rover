
//////////////////////////////////////////////////////////////////////////////
//-------------------- BRANCH -self test implimnetation --------------------//
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <string.h>
#include "transport.h"
#include "transport_uart.h"
#include "transport_fifo.h"
#include "global_config.h"
#include "debug.h"
#include "debug_config.h"



// defines the name od the debug macro for this file
#define DEBUG_FILE DBG_TRANSPORT 

// macro to calculate the size of an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


#define UART_BITS_PER_BYTE 10
#define TX_BYTE_TRANSMISSION_TIME ((UART_BITS_PER_BYTE *1000000UL) / COMS_PORT_BAUD)

#define SIZE_OF_HEAD 5 //START + TYPE + ACK + ID + DLC 
#define SIZE_OF_TAIL 1 // CRC 


// ===== PRIVATE PROTOTYPES =====
//static void copy_data_frame(struct frame *source, struct frame *destination);
static uint8_t inline_crc_calc(uint8_t CRC, uint8_t byte);
static bool msg_wdt_check();
static void reset_tx_values();
static void transmit_packet(struct frame *f);
static bool pack_message(uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, struct frame *ptr);
static void pack_ack(uint8_t type, uint8_t id, struct frame *f);
static void reset_rx_values(); 
static uint16_t frame_size(struct frame *f);



// read variables
uint8_t current_payload = 0; 
uint32_t last_read_byte = 0x00; 
uint8_t rx_crc_check = 0x00;
bool ack_received = false;

// send variables 
uint32_t tx_wdt_timestamp = 0; 
uint8_t tx_retries = 0;
bool send_ack_flag = false;
uint8_t packet_id = 0xAA; 
uint32_t fifo_delay_timer = 0; 


 enum TX_STATE{
  TX_STATE_IDLE,
  TX_STATE_SENDING,
  TX_STATE_AWAITING_ACK,
  TX_STATE_RETRY,
  TX_STATE_FIFO_DELAY,  // only used during self test 
  TX_STATE_SUCCESS,
  TX_STATE_FAILED // handle errors here log files ect. 
 } tx_state; 

enum RX_STATE{
  WAIT_START,
  READ_TYPE,
  READ_ACK,
  READ_ID,
  READ_DLC, 
  READ_PAYLOAD, 
  READ_CRC
  } rx_state; 

struct frame rx_frame;   // frame for receiving data
struct frame tx_frame;   // frame for transmitting data
struct frame ack_tx_frame;  // frame for transmitting ack
struct frame ack_rx_frame;  // frame for receiving ack
struct frame *active;       // active frame for sending data 

// communication port struct defined in transport.h and initialized in main.cpp 
Transport_IO *current_transport = DEFAULT_TRANSPORT; 


// frankenstine debug /////////////
uint8_t get_ava(){
   return current_transport->available();
}  
///////////////////////////////////

bool transport_set(Transport_IO *io){
  if(io == NULL){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "PORT", "Invalid transport IO provided; current_transport: ", "uart_io");
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

static bool msg_wdt_check(){
  if(rx_state > WAIT_START && (micros() - last_read_byte) > WDT_TIMEOUT_US ){
    //handle time out error here
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WDT", "over 10ms between byte transmission in satate: ", rx_state);
    reset_rx_values();
    return true;
  }
  else{
    return false; 
  }
  return false; // should not be reached
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

static void reset_rx_values(){
  flush_struct(&rx_frame);    // flush the rx frame struct to reset all values to 0x00.
  rx_crc_check = 0x00;        // reset the crc calculation
  current_payload = 0;        // reset the payload countet
  rx_state = WAIT_START; 
}

void reset_tx_values(){
      tx_retries = 0; 
      tx_wdt_timestamp = 0; 
         
      //Flush the TX & ACK 
      flush_struct(&tx_frame);
      flush_struct(&ack_tx_frame);
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

void transport_pack_and_send_packet(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data){

  ack_received = false; // clear the ack received flag before sending a new message.  
  
  // send ack frame
  if(ack == ACK_RESPONSE){
    pack_ack(type, rx_frame.ID, &ack_tx_frame);
    active = &ack_tx_frame;
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_ACK_MSG, START_BYTE, "ACK_PAC", ack_tx_frame);
    tx_state = TX_STATE_SENDING;
  }
  // send message frame
  if(pack_message(type, ack, packet_id, dlc, data, &tx_frame) == true){
    active = &tx_frame; 
    DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, START_BYTE, "TX_PAC", tx_frame);
    tx_state = TX_STATE_SENDING;
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
  f->CRC = 0; 

  // calculate CRC for data frame
  f->CRC = inline_crc_calc(f->CRC, f->TYPE);
  f->CRC = inline_crc_calc(f->CRC, f->ACK);
  f->CRC = inline_crc_calc(f->CRC, f->ID);
  f->CRC = inline_crc_calc(f->CRC, f->DLC);
  for(uint8_t i = 0; i < dlc; i++){ 
    f->payload[i] = data[i];
    f->CRC = inline_crc_calc(f->CRC, f->payload[i]);
  }  
  // debug print the packed frame before sendingg the frame.
  DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, START_BYTE, "PAC_PTR", f);
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
  *out = rx_frame;
  DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MSG", "receive frame returned to protocal layer");
}

static inline uint16_t frame_size(struct frame *f) {
    if (f == NULL) return 0;
    return SIZE_OF_HEAD + f->DLC + SIZE_OF_TAIL;
}

///////////////// RECEIVE DATA FRAME ////////////////////

uint8_t read_data_frame(){

  if (current_transport->available() > 0) {
    last_read_byte = micros();
    uint8_t incoming = current_transport->read();
    
    switch(rx_state){

      case WAIT_START: 
      if(incoming == START_BYTE){
        rx_crc_check = 0; 
        rx_state = READ_TYPE;
        DEBUG_PRINT_MSG_VAL_HEX(DEBUG_FILE, DEBUG_DEBUG, "RX", "start byte received: ", incoming);
        DEBUG_STREAM_START(DEBUG_FILE, DEBUG_STREAM, "RX", incoming);
        return START_RECEIVED;
      }
      else{
        return WAITING_FOR_START;
      }
      break;

      case READ_TYPE:
        rx_frame.TYPE = incoming;
        rx_crc_check = inline_crc_calc(rx_crc_check, incoming);
        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
        rx_state = READ_ACK;
        return NO_ERROR;
      break;

      case READ_ACK:
        rx_frame.ACK = incoming;
        rx_crc_check = inline_crc_calc(rx_crc_check, incoming);
        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
        

        if(rx_frame.ACK == ACK_REQUEST){
          send_ack_flag = true;
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK request received.");
          rx_state = READ_ID;
          return ACK_REQUESTED;
        }
        else if(rx_frame.ACK == NACK){
          send_ack_flag = false;
          rx_state = READ_ID;
          return NACK_REQUESTED;
        }
        else if(rx_frame.ACK == NORMAL_FRAME){
          send_ack_flag = false; 
          rx_state = READ_ID;
          return NORMAL_FRAME_RECEIVED;
        }
        else{
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "ACK", "ACK out of range ACK: ", rx_frame.ACK);
          reset_rx_values(); // reset message valvues to 0x00.
          return ACK_OUT_OF_RANGE;
        }
      break;

      case READ_ID: 
        rx_frame.ID = incoming;
        rx_crc_check = inline_crc_calc(rx_crc_check, incoming);
        DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
        rx_state = READ_DLC;
        return ID_RECEIVED; 
      break;

      case READ_DLC: 
        if(incoming > ARRAY_SIZE(rx_frame.payload)){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "RX", "DLC greater that payload capacity: ", ARRAY_SIZE(rx_frame.payload));
          reset_rx_values();
          return DLC_OVER_CAPACITY;
        }
        else{
          rx_frame.DLC = incoming;
          rx_crc_check = inline_crc_calc(rx_crc_check, incoming);
          rx_state = READ_PAYLOAD;
          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
          return DLC_RECEIVED;
        }
      break; 

      case READ_PAYLOAD:
        if(current_payload < rx_frame.DLC){
          rx_frame.payload[current_payload] = incoming;
          rx_crc_check = inline_crc_calc(rx_crc_check, incoming);
          current_payload ++; 
          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
        }
          
        if (current_payload == rx_frame.DLC){
          current_payload = 0; 
          rx_state = READ_CRC;
          return PAYLOAD_COMPLETE;
        }
        else if (current_payload > rx_frame.DLC){
          // error detected
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "DLC", "Payload exceded DLC: ", current_payload);
          reset_rx_values(); // reset message valvues to 0x00.
          return PAYLOAD_OVERFLOW; 
        }
        return RECEIVING_DATA;
      break;

      case READ_CRC:

        rx_frame.CRC = incoming;

        // check and report corupt frames
        if(rx_crc_check != rx_frame.CRC){
          DEBUG_STREAM_END(DEBUG_FILE, DEBUG_STREAM, incoming);
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "CRC", "CRC check NOK. calculated CRC: ", rx_crc_check);
          reset_rx_values(); // reset message valvues to 0x00 and read paramters 
          return CRC_ERROR; 
        }
        // action any frame that is confirmed good. 
        else{
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "CRC", "CRC check OK.");

          if(rx_frame.ACK == ACK_REQUEST){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "responding to ACK request.");
            pack_ack(rx_frame.TYPE, rx_frame.ID, &ack_tx_frame);
            transmit_packet(&ack_tx_frame);

            //transport_pack_and_send_packet(rx_frame.TYPE, rx_frame.ACK, rx_frame.DLC, rx_frame.payload);
          }
          else if(rx_frame.ACK == ACK_RESPONSE && rx_frame.ID == tx_frame.ID){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "received ACK response.");
            ack_received = true;
          } 
          
          rx_state = WAIT_START;
          DEBUG_STREAM_END(DEBUG_FILE, DEBUG_STREAM, incoming);
          DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, RX_FRAME, START_BYTE, "[RX]", rx_frame);
          return FRAME_READY; 
        }  
      break;
    }
  }

  // cehcek the message WTD for timeout error rest
  if (msg_wdt_check() == true){
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WDT", "message watch dog timer timed out. Elapsed time: ", (micros() - last_read_byte));
    return MSG_TIMEOUT_ERROR; 
  }
  else{
    return NO_DATA_AVAILABLE;
  }
}


///////////////// SEND DATA FRAME ////////////////////
 
uint8_t send_data_frame(){

  switch(tx_state){

    case TX_STATE_IDLE:
    return TX_IDLE_STATE;
    break; 
    

    case TX_STATE_SENDING: 
    {
      // frame selection moveted to pack_message() which frame will be trnasmitte

      //test active is not NULL before sending data
      if (active == NULL) {
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "PTR", "atcive transport pointer NULL active: NULL");
      tx_state = TX_STATE_FAILED;
      return TX_ERROR;
      }
 
      transmit_packet(active);
      DEBUG_PRINT_DATA_PTR_FRAME(DEBUG_FILE, DEBUG_MSG, TX_FRAME, START_BYTE, "TX_PTR", active);
      // check for fifo_io and switch to TX_FIFO_DELAY / 
      //this state then switches to TX_STATE_AWAITING_ACK ack or TX_STATE_SUCCESS
      if(current_transport == &fifo_io){
        fifo_delay_timer = micros();
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_DEBUG, "TX", "time stamp starting TX_STATE_FIFO_DELAY: ", micros());
        tx_state = TX_STATE_FIFO_DELAY;
        return TRANSMITTING;
      }

      if(active->ACK == ACK_REQUEST){ // function sent to awating ack before thedelay for fifo is ran.
        tx_wdt_timestamp = micros();
        tx_state = TX_STATE_AWAITING_ACK;
      }
      else{
          tx_state = TX_STATE_SUCCESS;
      }
      return TRANSMITTING;
    }
    break; 
    

    case TX_STATE_AWAITING_ACK:
      if(ack_received == true){
  
        ack_received = false; 
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "ACK",  "ACK received & validated for packet_id: ", rx_frame.ID); 

      }
      

      // handles time out and retries
      if((micros() - tx_wdt_timestamp) > ACK_WDT_VAL){
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR,"WDT", "ACK_WDT_VAL timeed out elapses time(us): ", micros() - tx_wdt_timestamp);
        if(tx_retries < TX_MAX_RETRIES){
          tx_retries ++;
          tx_state = TX_STATE_RETRY;
          return ACK_WDT_TIMEOUT;
        }
    
        else{
          DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_ERROR, "ACK", "ack not received after" ,(tx_retries - 1), "retries");
          tx_state = TX_STATE_FAILED; 
          return TX_RETRIES_FAILED;
        }
      }
      return AWAITING_ACK;
    break;
    

    case TX_STATE_RETRY: 
      DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "TX", "ACK watchdog timer timed out. retries: ", tx_retries);
      transmit_packet(active);
      tx_wdt_timestamp = micros();
      tx_state = TX_STATE_AWAITING_ACK; 
      return RESENDING_MSG;
    break;

    case TX_STATE_FIFO_DELAY:
    if (micros() - fifo_delay_timer > (frame_size(active) * TX_BYTE_TRANSMISSION_TIME)){ // change to dynamcic baud calc 
      tx_state = TX_STATE_SUCCESS;
      DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_DEBUG, "TX", "time stamp exiting TX_STATE_FIFO_DELAY: ", micros());
      
      if(active->ACK == ACK_REQUEST){ 
        tx_wdt_timestamp = micros();
        tx_state = TX_STATE_AWAITING_ACK; 
        return FIFO_DELAY_SUCSESSFUL;
      }
      else{
          tx_state = TX_STATE_SUCCESS;
          return FIFO_DELAY_SUCSESSFUL;
      }
    }
    else{
      DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "fifo delay inpregress remaining time:", (micros() - fifo_delay_timer) - (frame_size(active) * TX_BYTE_TRANSMISSION_TIME));
      return TX_FIFO_WAIT; 
    }
    break;

    case TX_STATE_SUCCESS:
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TX", "TX transmission successful.");

      packet_id += PACKET_INCREMENT; // *** chenge to ++ after testing ***

      reset_tx_values(); // reset all values to default and flush the tx and ack frames.
      tx_state = TX_STATE_IDLE;
      return TX_SUCCESS;
    break;
    

    case TX_STATE_FAILED:
      // HAndle error reporting / logging here before returniing the state. loging to be handles with debug functiond and passes to a writing buffer.
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "ACK", "TX_PACKET failed to send");

      packet_id += PACKET_INCREMENT; // *** chenge to ++ after testing ***

      reset_tx_values(); // reset all values to defualt and flush the tx and ack frames.
      tx_state = TX_STATE_IDLE;
      return TX_ERROR;
    break;
      
  };
  return TX_IDLE_STATE; // default return value should never be reached.
}