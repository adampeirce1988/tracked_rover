
#include <Arduino.h>
#include "transport_uart.h"
#include "transport.h"
#include "global_config.h"
//#include "debug_config.h"
#include "debug.h"

// define statments 
#define DEBUG_FILE           DBG_TRANSPORT_FIFO             //debug file name 
#define BUFFER_SIZE          FIFO_BUFFER_SIZE               // local macro set by global_config 
#define BITS_PER_FRAME       10                             // number of bytes in 8N1 transmission 
#define MICORS_PER_SECOND    1000000UL                      // no of u_seconds per second 

//Forward declarations
void fifo_write(uint8_t byte);
uint8_t fifo_read();
uint8_t fifo_available();
uint8_t fifo_begin(uint32_t baud_rate);
void fifo_update();


//uart struct instance
Transport_IO fifo_io = {
    fifo_write,
    fifo_available,
    fifo_read,
    fifo_begin,
    fifo_update
};

// fifo ring buffer
uint32_t outgoing_transmission_timer = 0; 
uint32_t last_tx_engine_update = 0; 

uint8_t tx_ring_buffer[FIFO_BUFFER_SIZE];     // tx ring buffer 
uint8_t tx_head = 0;                          // tx Ring buffer head
uint8_t tx_tail = 0;                          // tx ring buffer tail

uint8_t rx_ring_buffer[FIFO_BUFFER_SIZE];     // rx ring buffer
uint8_t rx_head = 0;                          // Ring buffer head
uint8_t rx_tail = 0;                          // Ring buffer tail 
    
 
// functions 
void fifo_write(uint8_t byte){ 
    if(((tx_head + 1) % BUFFER_SIZE) == tx_tail){
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "FIFO", "FIFO tx buffer overflow tx byte droped. Byte: ", byte);
    }
    else{
        tx_ring_buffer[tx_head] = byte;  // write a byte to the tx_ring_buffer ready to send. 
        tx_head = (tx_head + 1) % BUFFER_SIZE;
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "byte writen to the tx_ring_buffer. Byte: ", byte); 
    } 
}

uint8_t fifo_available(){
  uint8_t available = (rx_head + BUFFER_SIZE - rx_tail) % BUFFER_SIZE;
  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "data avalaibel to read. avaliable: ", available);
  return available; 
}


uint8_t fifo_read(){
    if(rx_tail == rx_head){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "FIFO", "No data in rx_ring_buffer to read");
    }
    else{ 
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "data read from tx_ring buffer: ", rx_ring_buffer[rx_tail]);
        uint8_t byte  = rx_ring_buffer[rx_tail] ;
        rx_tail = (rx_tail +1) % BUFFER_SIZE;
        return byte;
    }
    return 0;
}

uint8_t fifo_begin(uint32_t baud_rate){
    outgoing_transmission_timer = (uint32_t)(((BITS_PER_FRAME * MICORS_PER_SECOND) / baud_rate) + 0.5f);
    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_INFO, "FIFO", "outgoing_transmission_timer set to" , outgoing_transmission_timer, " us.");
    return 1; 
}

void fifo_update(){
    if(tx_head != tx_tail){ // data avalaible to read

        uint8_t next_rx_head = (rx_head + 1) % BUFFER_SIZE; 

        if(next_rx_head == rx_tail){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "rx_ring_buffer overflow detected");
        }
        else{
            if((micros() - last_tx_engine_update) >= outgoing_transmission_timer){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "fifo uart engine updated");
                rx_ring_buffer[rx_head] = tx_ring_buffer[tx_tail];
                last_tx_engine_update += outgoing_transmission_timer; 
                rx_head = (rx_head + 1) % BUFFER_SIZE;
                tx_tail = (tx_tail + 1) % BUFFER_SIZE; 
            }
            else{
                DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "remaing time until next transmission: ", outgoing_transmission_timer - (micros() - last_tx_engine_update), "us.");
            }
        }
    }
    else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "no data avaliable to transmsmit in the tx_buffer.");
    }
}

