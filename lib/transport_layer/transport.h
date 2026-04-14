#ifndef TRANSPORT_LAYER_H
#define TRANSPORT_LAYER_H

#include <Arduino.h>
#include <stdint.h>

// ================= CONFIG =================
#define START_BYTE        0xFF
#define WDT_TIMEOUT_MS    4
#define MAX_PAYLOAD_LEN   6
#define PACKET_INCREMENT  1
#define TX_MAX_RETRIES    3
#define ACK_WDT           10

// // move the to global config file later
// #define SERIAL_0_BAUD     115200
// #define SERIAL_1_BAUD     115200

// #define DEBUG_PORT        Serial
// #define COMS_PORT         Serial1

// ================= ACK TYPES =================
#define NORMAL_FRAME      0x00
#define ACK_REQUEST       0x01
#define ACK_RESPONSE      0x02
#define NACK              0x03

// ================= RX RETURN CODES =================
#define NO_ERROR                  0
#define START_RECEIVED            1
#define INVALID_TYPE              2
#define ACK_OUT_OF_RANGE          3
#define ACK_REQUESTED             4
#define NACK_REQUESTED            5
#define DLC_OVER_CAPACITY         6
#define PAYLOAD_OVERFLOW          7
#define MSG_TIMEOUT_ERROR         8
#define ACK_MISSING               9
#define CRC_ERROR                 10
#define RECEIVING_DATA           11
#define NO_DATA_AVAILABLE        12
#define FRAME_READY              13
#define DLC_RECEIVED             14

// ================= TX RETURN CODES =================
#define TX_IDLE_STATE      0
#define TRANSMITTING       1
#define AWAITING_ACK       2
#define RESENDING_MSG      3
#define TX_SUCCESS         5
#define TX_ERROR           6

// ================= STRUCT =================
struct frame {
  uint8_t TYPE;
  uint8_t ACK;
  uint8_t ID;
  uint8_t DLC;
  uint8_t payload[MAX_PAYLOAD_LEN];
  uint8_t CRC;
};

// ================= API =================
void send_packet(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
bool get_received_frame(struct frame *out);
void com_port_open();
void debug_port_open();
uint8_t read_data_frame();
uint8_t send_data_frame();
void print_frame(struct frame *f);

#endif