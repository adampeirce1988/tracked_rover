#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <Arduino.h>
#include <stdint.h>
#include "global_config.h"
#include "transport_types.h"


// ================= MACROS =================
//transport macros 
#define FRAME_SIZE(f) (sizeof(struct frame) - MAX_PAYLOAD_LEN + (f).DLC)

// fifo macros 
#define MAX_FRAME_SIZE(f) (sizeof (struct frame))
#define DYNAMIC_FRAME_SIZE(f) (sizeof(struct frame) - MAX_PAYLOAD_LEN + (f).DLC)

// ================= START BYTE ================
// constexpr uint8_t START_BYTE =                0xFF;

// ================= STRUCT =================

// //data frame structure
// struct frame {
//   uint8_t TYPE;
//   uint8_t ACK;
//   uint8_t ID;
//   uint8_t DLC;
//   uint8_t payload[MAX_PAYLOAD_LEN];
//   uint8_t CRC;
// };


// port structure
// struct Transport_IO {
//   void (*write)( uint8_t );
//   uint8_t (*available)();
//   uint8_t (*read)();
//   uint8_t (*begin)(uint32_t);
//   void (*update)(void);
// };


// extern Transport_IO fifo_io; 
// extern Transport_IO uart_io; 


// ================= API function calls =================
//transport function calls 

// fifo uart io
//void coms_port_begin(uint32_t baud_rate);
// void fifo_io_uart_engine_update(); 

// //Diagnostics
// void set_tx_fault_injection_active(TX_SET_FAULT_MODE type, uint8_t value);              // must be set each time i fault is to be injected i to the system.
// void tx_frame_error_injection(frame *f);                                                // injection point for faults in inside pack messag after packing and crc are calculated. 
// uint16_t transport_get_tx_latancy();                                                    // should only be called once FRAME_READY is confimed 
// uint16_t transport_get_rx_latancy();                                                    // should only be called once TX_TRANSMISSION_SUCSESS 

// transport 
//void com_port_open();
// bool transport_set(Transport_IO *io);
//bool transport_set_fifo();                                                              // ADDED to remove exposure of Transport_IO in simulation.h
// bool transport_set_uart();  
//bool transport_set_default();                                                            // ADDED to remove exposure of Transport_IO in simulation.h
// void transport_queue_message(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
// void transport_get_frame(struct frame *out);
// uint8_t frame_avaliable(); 


////////////////////////////////////////
//         EXTERNAL CALLS             //
////////////////////////////////////////

bool ports_init(uint32_t baud_rate);    // sets up both the fifo & uart
bool transport_set_fifo();                                                              // ADDED to remove exposure of Transport_IO in simulation.h
bool transport_set_uart();  
bool transport_set_default();  
void transport_queue_message(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);
void transport_get_frame(struct frame *out);
uint8_t frame_avaliable(); 

// run simulation engine 
void fifo_io_uart_engine_update(); 

//Diagnostics
void set_tx_fault_injection_active(TX_SET_FAULT_MODE type, uint8_t value);              // must be set each time i fault is to be injected i to the system.
void tx_frame_error_injection(frame *f);                                                // injection point for faults in inside pack messag after packing and crc are calculated. 
uint16_t transport_get_tx_latancy();                                                    // should only be called once FRAME_READY is confimed 
uint16_t transport_get_rx_latancy();                                                    // should only be called once TX_TRANSMISSION_SUCSESS 


// update state machines must be called in order each loop 
RX_RETURN_CODES update_rx_fsm();
TX_RETURN_CODES update_tx_fsm();



#endif