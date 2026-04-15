#include "transport.h"
#include "global_config.h"
#include "debug.h"
#include "debug_config.h"


// defines the name od the debug macro for this file
#define DEBUG_FILE DBG_TRANSPORT 

// macro to calculate the size of an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


// ===== PRIVATE PROTOTYPES =====
static void copy_data_frame(struct frame *source, struct frame *destination);
static uint8_t inline_crc_calc(uint8_t CRC, uint8_t byte);
static bool msg_wdt_check();
static void reset_tx_values();
static void transmit_packet(struct frame *f);
static void pack_message(uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, struct frame *ptr);
static void pack_ack(uint8_t type, uint8_t id, struct frame *f);
static void send_ack(uint8_t type, uint8_t id);
static void reset_rx_values(); 


// read variables
uint8_t current_payload = 0; 
uint32_t last_read_byte = 0; 
uint8_t rx_crc_check = 0x00;
bool ack_reqested_flag = false;

// send variables 
uint32_t tx_timestamp = 0; 
uint8_t tx_retries = 0; 
bool ack_received = false; 
uint8_t packet_id = 0; 


 enum TX_STATE{
  TX_IDLE,
  TX_SENDING,
  TX_AWAITING_ACK,
  TX_RETRY,
  TX_STATE_SUCCESS,
  TX_FAILED // handle errors here log files ect. 
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

static void copy_data_frame(struct frame *source, struct frame *destination){
  destination->TYPE = source->TYPE;
  destination->ACK = source->ACK;
  destination->ID = source->ID;
  destination->DLC = source->DLC;
  for(uint8_t i = 0; i < source->DLC; i++){
    destination->payload[i] = source->payload[i];
  }
  destination->CRC = source->CRC;
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
  if(rx_state > WAIT_START && (millis() - last_read_byte) > WDT_TIMEOUT_MS ){
    //handle time out error here
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WDT", "over 10ms between byte transmission in satate: ", rx_state);

    reset_tx_values();
    return true;
  }
  else{
    return false; 
  }
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
  flush_struct(&rx_frame);
  rx_crc_check = 0x00;   // reset the crc calculation
  current_payload = 0; 
  rx_state = WAIT_START; 
}

void reset_tx_values(){
      tx_retries = 0; 
      tx_timestamp = 0; 
      ack_received = false;

      //Flush the TX & ACK 
      flush_struct(&tx_frame);
      flush_struct(&ack_tx_frame);
}

static void transmit_packet(struct frame *f){
  
  COMS_PORT.write(START_BYTE); 
  COMS_PORT.write(f->TYPE);
  COMS_PORT.write(f->ACK);
  COMS_PORT.write(f->ID);
  COMS_PORT.write(f->DLC);

  for(uint8_t i = 0; i < f->DLC; i++){
    COMS_PORT.write(f->payload[i]);
  }

  COMS_PORT.write(f->CRC);
}

void send_packet(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data){

  pack_message(type, ack, packet_id, dlc, data, &tx_frame);
  DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, START_BYTE, "TX_PAC", tx_frame);
  tx_state = TX_SENDING;
}

static void pack_message(uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, struct frame *f){

  flush_struct(f); // flush the struct before packaging the frame  

  f->TYPE = type;
  f->ACK = ack;
  f->ID = id;
  f->DLC = dlc;
  f->CRC = 0; 
  
  f->CRC = inline_crc_calc(f->CRC, f->TYPE);
  f->CRC = inline_crc_calc(f->CRC, f->ACK);
  f->CRC = inline_crc_calc(f->CRC, f->ID);
  f->CRC = inline_crc_calc(f->CRC, f->DLC);
  
  for(uint8_t i = 0; i < dlc; i++){
    f->payload[i] = data[i];
    f->CRC = inline_crc_calc(f->CRC, f->payload[i]);
  }  
  
  DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_PACK_MSG, START_BYTE, "PAC", tx_frame);

}

static void pack_ack(uint8_t type, uint8_t id, struct frame *f){

  flush_struct(f); // flush the ack struct before packaging the frame. 

  f->TYPE = type;
  f->ACK = ACK_RESPONSE;
  f->ID = id;
  f->DLC = 0x00; // no data to sent in ACK.
  f->CRC = 0; 
  
  f->CRC = inline_crc_calc(f->CRC, f->TYPE);
  f->CRC = inline_crc_calc(f->CRC, f->ACK);
  f->CRC = inline_crc_calc(f->CRC, f->ID);
  f->CRC = inline_crc_calc(f->CRC, f->DLC);  
}

static void send_ack(uint8_t type, uint8_t id){
  pack_ack(type, id, &ack_tx_frame);
  DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, ACK_FRAME, START_BYTE, "ACK", ack_tx_frame);
  tx_state = TX_SENDING; 
}

bool get_received_frame(struct frame *out){
  *out = rx_frame;
  DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MSG", "receive frame returned to protocal layer");
  return true; 
}

void com_port_open(){
  COMS_PORT.begin(COMS_PORT_BAUD); 
  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "PORT", "communication port stated at: ", COMS_PORT_BAUD);
}

uint8_t read_data_frame(){

  if (COMS_PORT.available() > 0) {
    last_read_byte = millis();
    uint8_t incoming = COMS_PORT.read();
    
    switch(rx_state){

      case WAIT_START: 
      if(incoming == START_BYTE){
        rx_crc_check = 0; 
        rx_state = READ_TYPE;
        DEBUG_PRINT_MSG_VAL_HEX(DEBUG_FILE, DEBUG_DEBUG, "RX", "start byte received: ", incoming);
        DEBUG_STREAM_START(DEBUG_FILE, DEBUG_STREAM, "RX", incoming);
        return START_RECEIVED;
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
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK request received.");
          return ACK_REQUESTED;
        }
        else if(rx_frame.ACK == NACK){
          rx_state = READ_ID;
          return NACK_REQUESTED;
        }
        else if(rx_frame.ACK == NORMAL_FRAME){
          rx_state = READ_ID;
          return NORMAL_FRAME_RECEIVED;
        }
        else{
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "ACK", "ACK out of range ACK: ", rx_frame.ACK);
          reset_tx_values();
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
          reset_tx_values();
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
          DEBUG_STREAM_DATA(DEBUG_FILE, DEBUG_STREAM, incoming);
          return PAYLOAD_COMPLETE;
        }
        else if (current_payload > rx_frame.DLC){
          // error detected
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "DLC", "Payload exceded DLC: ", current_payload);
          reset_rx_values(); // reset message valvues to 0x00 and read paramters 
          return PAYLOAD_OVERFLOW; 
        }
        return RECEIVING_DATA;
      break;

      case READ_CRC:
        rx_frame.CRC = incoming;
        
        if(rx_crc_check != rx_frame.CRC){
          DEBUG_STREAM_END(DEBUG_FILE, DEBUG_STREAM, incoming);
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "CRC", "CRC check NOK. calculated CRC: ", rx_crc_check);
          reset_rx_values(); // reset message valvues to 0x00 and read paramters 
          return CRC_ERROR; 
        }
        else{
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "CRC", "CRC check OK.");

          if(rx_frame.ACK == ACK_REQUEST){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "ACK", "responding to ACK request.");
            send_ack(rx_frame.TYPE, rx_frame.ID);
          }
          else if(rx_frame.ACK == ACK_RESPONSE && rx_frame.ID == tx_frame.ID){
            ack_received = true; 
            copy_data_frame(&rx_frame, &ack_rx_frame);
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
    return MSG_TIMEOUT_ERROR; 
  }
  else{
    return NO_DATA_AVAILABLE;
  }
}

uint8_t send_data_frame(){

  switch(tx_state){

    case TX_IDLE:
    return TX_IDLE_STATE;
    break; 

    case TX_SENDING: 
      transmit_packet(&tx_frame);

      DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, TX_FRAME, START_BYTE, "TX", tx_frame);

      if(tx_frame.ACK == ACK_REQUEST){
        tx_timestamp = millis();
        tx_state = TX_AWAITING_ACK; 
      }
      else{
          tx_state = TX_STATE_SUCCESS;
      }
      return TRANSMITTING;
    break; 

    case TX_AWAITING_ACK:
      if(ack_received == true){
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "ACK",  "ACK received for packet_id: ", tx_frame.ID); 
        if((ack_rx_frame.TYPE == tx_frame.TYPE) && (ack_rx_frame.ID == tx_frame.ID)){
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "ACK", "ACK received and type matches. ACK type: ", ack_rx_frame.TYPE);
          return TX_SUCCESS;
          tx_state = TX_STATE_SUCCESS; 
        }
        else{
          DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "ACK", "ACK received but type mismatch. ACK type: ", ack_rx_frame.TYPE);
          tx_state = TX_FAILED;
        }
      }
      // handles time out and retries
      if((millis() - tx_timestamp) > ACK_WDT){
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_DEBUG,"WDT", "ACK_WDT timeed out elapses time: ", millis() - tx_timestamp);
        if(tx_retries < TX_MAX_RETRIES){
          tx_retries ++;
          tx_state = TX_RETRY;
        }
        else{
          tx_state = TX_FAILED; 
        }
      }
      return AWAITING_ACK;
    break;

    case TX_RETRY: 
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "TX_RETRY", "Attempting to resend message.");

      transmit_packet(&tx_frame);
      tx_timestamp = millis();
      tx_state = TX_AWAITING_ACK; 
      return RESENDING_MSG;
    break; 

    case TX_STATE_SUCCESS:
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "TX", "TX transmission successful.");

      packet_id += PACKET_INCREMENT;
      reset_tx_values(); // reset all values to default and flush the tx and ack frames.
      tx_state = TX_IDLE;
      return TX_SUCCESS;
    break;

    case TX_FAILED:
      // HAndle error reporting / logging here before returniing the state. loging to be handles with debug functiond and passes to a writing buffer.
      DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "ACK", "ACK not received. no of retries: ", tx_retries);

      packet_id += PACKET_INCREMENT;

      reset_tx_values(); // reset all values to defualt and flush the tx and ack frames.
      tx_state = TX_IDLE;
      return TX_ERROR;
    break; 
  };
  return TX_IDLE_STATE; // default return value should never be reached.
}