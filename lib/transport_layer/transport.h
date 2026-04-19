#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <Arduino.h>
#include <stdint.h>

// ================= MACROS =================
#define FRAME_SIZE(f) (sizeof(struct frame) - MAX_PAYLOAD_LEN + (f).DLC)

// ================= CONFIG =================
#define START_BYTE                0xFF
#define WDT_TIMEOUT_MS            10
#define MAX_PAYLOAD_LEN           6
#define PACKET_INCREMENT          1
#define TX_MAX_RETRIES            3
#define ACK_WDT                   25

// ================= FIFO =================


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
#define ACK_REQUESTED             5
#define NACK_REQUESTED            6
#define NORMAL_FRAME_RECEIVED     7
#define ID_RECEIVED               8
#define DLC_OVER_CAPACITY         9
#define PAYLOAD_OVERFLOW          10
#define PAYLOAD_COMPLETE          11
#define MSG_TIMEOUT_ERROR         12
#define ACK_MISSING               13
#define CRC_ERROR                 14
#define RECEIVING_DATA            15
#define NO_DATA_AVAILABLE         16
#define FRAME_READY               17
#define DLC_RECEIVED              18
#define FIFO_DELAY_SUCSESSFUL     19

// ================= TX RETURN CODES =================
#define TX_IDLE_STATE             0
#define TRANSMITTING              1
#define AWAITING_ACK              2
#define TYPE_MISMATCH             3
#define ACK_WDT_TIMEOUT           4
#define TX_RETRIES_FAILED         5
#define RESENDING_MSG             6
#define TX_SUCCESS                7
#define TX_ERROR                  8
#define TX_FIFO_WAIT              9


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
  void (*begin)(uint32_t);
};

// ================= API function calls =================

// franinstine debug 
uint8_t get_ava();

//transport function calls 
void coms_port_begin(uint32_t baud_rate);
bool transport_set(Transport_IO *io);
void transport_send_packet(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
void transport_get_frame(struct frame *out);
void com_port_open();
uint8_t read_data_frame();
uint8_t send_data_frame();
void print_frame(struct frame *f);

#endif