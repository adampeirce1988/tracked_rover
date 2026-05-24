#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <Arduino.h>
#include <stdint.h>
#include "global_config.h"


// ================= MACROS =================
//transport macros 
#define FRAME_SIZE(f) (sizeof(struct frame) - MAX_PAYLOAD_LEN + (f).DLC)

// fifo macros 
#define MAX_FRAME_SIZE(f) (sizeof (struct frame))
#define DYNAMIC_FRAME_SIZE(f) (sizeof(struct frame) - MAX_PAYLOAD_LEN + (f).DLC)

// ================= START BYTE ================
constexpr uint8_t START_BYTE =                0xFF;

// ================= ACK TYPES =================
constexpr uint8_t NORMAL_FRAME =              0x00;
constexpr uint8_t ACK_REQUEST  =              0x01;
constexpr uint8_t ACK_RESPONSE =              0x02;
constexpr uint8_t NACK =                      0x03;

// ================= RX RETURN CODES =================
namespace RX_RETURN_CODES{
  constexpr uint8_t RX_STATE_IDLE =           0;
  constexpr uint8_t WAITING_FOR_START =       1;
  constexpr uint8_t START_RECEIVED =          2;
  constexpr uint8_t TYPE_RECEIVED =           3;
  constexpr uint8_t ACK_REQUEST_RECEIVED =    4;
  constexpr uint8_t ACK_RESPONSE_RECEIVED =   5;
  constexpr uint8_t NACK_REQUEST_RECEIVED =   6;
  constexpr uint8_t NORMAL_FRAME_RECEIVED =   7;
  constexpr uint8_t ACK_RESPONSE_SENT =       8;
  constexpr uint8_t ID_RECEIVED =             9;
  constexpr uint8_t DLC_RECEIVED =            10;
  constexpr uint8_t RECEIVING_DATA =          11;
  constexpr uint8_t PAYLOAD_COMPLETE =        12;
  constexpr uint8_t FRAME_READY =             13;
  constexpr uint8_t ACK_READY =               14;
  constexpr uint8_t INVALID_TYPE =            15;
  constexpr uint8_t ACK_OUT_OF_RANGE =        16;
  constexpr uint8_t DLC_OVER_CAPACITY =       17;
  constexpr uint8_t PAYLOAD_OVERFLOW =        18;
  constexpr uint8_t CRC_ERROR =               19;
  constexpr uint8_t MSG_TIMEOUT_ERROR =       20;
};

// ================= TX RETURN CODES =================
namespace TX_RETURN_CODES{
  constexpr uint8_t TX_IDLE_STATE =           0;
  constexpr uint8_t TX_PENDING_ACK =          1;
  constexpr uint8_t TX_TRANSMITING =          2;
  constexpr uint8_t RESENDING_MSG =           3;
  constexpr uint8_t TX_TRANSMISION_SUCCESS =  4;
  constexpr uint8_t TX_TRANSMISION_ERROR =    5;
  constexpr uint8_t ACK_NOT_RECEVIED =        6;
  constexpr uint8_t ACK_MISMATCHED =          7;
  constexpr uint8_t ACK_WDT_TIMEOUT =         8;
  constexpr uint8_t TX_BUFFER_OVERFLOW =      9;
};

// =============== ENUM CLASS ===============
// global class used to select a test. 
enum class TX_SET_FAULT_MODE{           
  NONE,
  TYPE_CHANGE,
  ACK_CHANGE,
  ID_CHANGE,
  DLC_CHANGE,
  DLC_OVER_MAX_CAPACITY,
  CRC_RAND_FLIP_BIT, 
  CRC_CHANGE, 
  RAND_DATA_FLIP_BIT,
  RAND_DATA_CHANGE
}; 

// ================= STRUCT =================

//data frame structure
struct frame {
  uint8_t TYPE;
  uint8_t ACK;
  uint8_t ID;
  uint8_t DLC;
  uint8_t payload[MAX_PAYLOAD_LEN];
  uint8_t CRC;
};


// port structure
struct Transport_IO {
  void (*write)( uint8_t );
  uint8_t (*available)();
  uint8_t (*read)();
  uint8_t (*begin)(uint32_t);
  void (*update)(void);
};


extern Transport_IO fifo_io; 
extern Transport_IO uart_io; 


// ================= API function calls =================
//transport function calls 

// fifo uart io
void coms_port_begin(uint32_t baud_rate);
void fifo_io_uart_engine_update(); 

//Diagnostics
void set_tx_fault_injection_active(TX_SET_FAULT_MODE type, uint8_t value);    // must be set each time i fault is to be injected i to the system.
void tx_frame_error_injection(frame *f);   // injection point for faults in inside pack messag after packing and crc are calculated. 
uint16_t get_tx_latancy(); // should only be called once FRAME_READY is confimed 
uint16_t get_rx_latancy(); // should only be called once TX_TRANSMISSION_SUCSESS 

// transport 
void com_port_open();
bool transport_set(Transport_IO *io);
void transport_queue_message(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
void transport_get_frame(struct frame *out);
uint8_t frame_avaliable(); 

// update state machines must be called in order each loop 
uint8_t update_rx_fsm();
uint8_t update_tx_fsm();

#endif