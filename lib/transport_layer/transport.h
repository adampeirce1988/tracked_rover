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

#define START_BYTE                0xFF

// ================= ACK TYPES =================
#define NORMAL_FRAME              0x00
#define ACK_REQUEST               0x01
#define ACK_RESPONSE              0x02
#define NACK                      0x03

// ================= RX RETURN CODES =================
#define RX_STATE_IDLE             0
#define WAITING_FOR_START         1
#define START_RECEIVED            2
#define TYPE_RECEIVED             3
#define ACK_REQUEST_RECEIVED      4
#define ACK_RESPONSE_RECEIVED     5
#define NACK_REQUEST_RECEIVED     6
#define NORMAL_FRAME_RECEIVED     7
#define ACK_RESPONSE_SENT         8
#define ID_RECEIVED               9
#define DLC_RECEIVED              10
#define RECEIVING_DATA            11
#define PAYLOAD_COMPLETE          12
#define FRAME_READY               13
#define ACK_READY                 14
#define INVALID_TYPE              15
#define ACK_OUT_OF_RANGE          16
#define DLC_OVER_CAPACITY         17
#define PAYLOAD_OVERFLOW          18
#define CRC_ERROR                 19
#define MSG_TIMEOUT_ERROR         20


// ================= TX RETURN CODES =================
#define TX_IDLE_STATE             0
#define TX_PENDING_ACK            1
#define TX_TRANSMITING            2
#define RESENDING_MSG             3
#define TX_SUCCESS                4
#define TX_ERROR                  5
#define ACK_NOT_RECEVIED          6
#define ACK_MISMATCHED            7 
#define ACK_WDT_TIMEOUT           8
#define TX_BUFFER_OVERFLOW        9




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

// transport 
void com_port_open();
bool transport_set(Transport_IO *io);
void transport_queue_message(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
void transport_get_frame(struct frame *out);

// update state machines must be called in order each loop 
uint8_t update_rx_fsm();
uint8_t update_tx_fsm();






#endif