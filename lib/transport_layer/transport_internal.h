// #ifndef TRANSPORT_INTERNAL_H
// #define TRANSPORT_INTERNAL_H

// #include "transport.h"
// #include "global_config.h"

// enum TX_STATE{
//   TX_STATE_IDLE,
//   TX_STATE_SENDING,
//   TX_STATE_RETRY,
//   TX_STATE_SUCCESS,
//   TX_STATE_FAILED // handle errors here log files ect. 
//  } tx_state; 

// enum RX_STATE{
//   RX_STATE_WAIT_START,
//   RX_STATE_READ_TYPE,
//   RX_STATE_READ_ACK,
//   RX_STATE_READ_ID,
//   RX_STATE_READ_DLC, 
//   RX_STATE_READ_PAYLOAD, 
//   RX_STATE_READ_CRC,
//   RX_STATE_ERROR
//   } rx_state; 

// // port structure
// struct Transport_IO {
//   void (*write)( uint8_t );
//   uint8_t (*available)();
//   uint8_t (*read)();
//   uint8_t (*begin)(uint32_t);
//   void (*update)(void);
// };


// Transport_IO fifo_io; 
// Transport_IO uart_io; 

// struct frame *active;                                  // active frame for sending data 
// Transport_IO *current_transport = DEFAULT_TRANSPORT;   // communication port struct defined in transport.h and initialized in main.cpp

// // port structure
// struct Transport_IO {
//   void (*write)( uint8_t );
//   uint8_t (*available)();
//   uint8_t (*read)();
//   uint8_t (*begin)(uint32_t);
//   void (*update)(void);
// };

// struct frame {
//   uint8_t TYPE;
//   uint8_t ACK;
//   uint8_t ID;
//   uint8_t DLC;
//   uint8_t payload[MAX_PAYLOAD_LEN];
//   uint8_t CRC;
// };

// struct transmit_packet{
//   struct frame f;  
//   bool waiting = false;
//   uint8_t error_code = 0;
// };

// struct transmit_ack_packet{
//   struct frame f;
//   bool waiting = false; 
//   bool retry = false; 
//   bool failed = false; 
//   bool ack_confirmation = false; 
//   uint32_t ack_timestamp = 0; 
//   uint8_t retry_counter = 0; 
//   uint8_t error_code = 0; 
// };

// struct receive_packet{
//   struct frame f; 
//   uint8_t crc_check = 0; 
//   uint8_t payload_position = 0; 
//   uint8_t error_code = 0; 
//   bool ack_response = false; 
//   bool frame_ready = false; 
// };

// struct receive_ack{
//   uint8_t TYPE = 0x00; 
//   uint8_t ID = 0x00;
//   bool ack_valid = false; 
// };


// // structure to send a priority frame. 
// struct transmit_packet tx_priority_packet;
// struct transmit_packet tx_normal_packet; 
// struct transmit_ack_packet tx_pending_ack;
// struct receive_packet rx_packet; 
// struct receive_ack rx_ack;

// // global variables
// static uint32_t last_read_byte = 0x00; 
// static uint32_t rx_wdt_timestamp = 0; 
// static uint8_t packet_id = 0x01; 

// // Timestamps(us) 
//   uint32_t tx_start_timestamp = 0; 
//   uint32_t rx_start_timestamp = 0;
//   uint32_t total_rx_frame_time = 0;
//   uint32_t total_tx_frame_time = 0;

// #endif