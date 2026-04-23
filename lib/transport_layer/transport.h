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

// ================= CONFIG ================= // moved to global config 
// #define START_BYTE                0xFF
// #define WDT_TIMEOUT_US            100
// #define MAX_PAYLOAD_LEN           6
// #define PACKET_INCREMENT          1
// #define TX_MAX_RETRIES            3
// #define ACK_WDT                   25



// ================= ACK TYPES =================
#define NORMAL_FRAME              0x00
#define ACK_REQUEST               0x01
#define ACK_RESPONSE              0x02
#define NACK                      0x03

// ================= RX RETURN CODES =================
#define NO_ERROR                  0
#define START_RECEIVED            1
#define WAITING_FOR_START         2
#define INVALID_TYPE              3
#define ACK_OUT_OF_RANGE          4
#define ACK_REQUEST_RECEIVED      5
#define ACK_RESPONSE_RECEIVED     6
#define NACK_REQUEST_RECEIVED     7
#define NORMAL_FRAME_RECEIVED     8
#define ACK_RESPONSE_SENT         9
#define ID_RECEIVED               10
#define DLC_OVER_CAPACITY         11
#define PAYLOAD_OVERFLOW          12
#define PAYLOAD_COMPLETE          13
#define MSG_TIMEOUT_ERROR         14
#define ACK_MISSING               15
#define CRC_ERROR                 16
#define RECEIVING_DATA            17
#define NO_DATA_AVAILABLE         18
#define FRAME_READY               19
#define DLC_RECEIVED              20
#define RX_RECEIVED_ACK_OK        21

// ================= TX RETURN CODES =================
#define TX_IDLE_STATE             0
#define TRANSMITTING              1
#define AWAITING_ACK              2
#define RECEIVED_ACK              3
#define TYPE_MISMATCH             4
#define ACK_WDT_TIMEOUT           5
#define TX_RETRIES_FAILED         6
#define RESENDING_MSG             7
#define TX_SUCCESS                8
#define TX_ERROR                  9
#define TX_FIFO_WAIT              10


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


// franinstine debug 
uint8_t get_ava();

//transport function calls 
void coms_port_begin(uint32_t baud_rate);
bool transport_set(Transport_IO *io);
void transport_pack_and_send_packet(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
void transport_get_frame(struct frame *out);
void com_port_open();
uint8_t read_data_frame();
uint8_t send_data_frame();
void print_frame(struct frame *f);
void fifo_io_uart_engine_update(); 



#endif