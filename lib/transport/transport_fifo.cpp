
#include <Arduino.h>
#include "transport.h"
#include "transport_types.h"
#include "transport_internal.h"
#include "global_config.h"
#include "debug.h"

/*=============================================================================
    Debug Configuration
=============================================================================*/

#define DEBUG_FILE DBG_TRANSPORT_FIFO

/*=============================================================================
    FIFO Timing Constants
=============================================================================*/

constexpr uint8_t  BITS_PER_FRAME     = 10;           // number of bytes in 8N1 transmission 
constexpr uint32_t MICROS_PER_SECOND  = 1000000UL;    // no of u_seconds per second 


/*=============================================================================
    Private Function Prototypes
=============================================================================*/

void fifo_write(uint8_t byte);
uint8_t fifo_read();
uint8_t fifo_available();
uint8_t fifo_begin(uint32_t baud_rate);
void fifo_update();

/*=============================================================================
    Transport Interface
=============================================================================*/

Transport_IO fifo_io = {
    fifo_write,
    fifo_available,
    fifo_read,
    fifo_begin,
    fifo_update
};

struct RingBuffer{
    uint8_t buffer[MAX_SERIAL_BUFFER_SIZE];
    uint8_t head = 0;
    uint8_t tail = 0; 
};


/*=============================================================================
    Runtime Objects
=============================================================================*/

// UART timing simulation
uint32_t outgoing_transmission_timer = 0; 
uint32_t last_tx_engine_update       = 0; 

RingBuffer tx_ring_buffer; 
RingBuffer rx_ring_buffer; 

   
/*=============================================================================
    FIFO Ring Buffer functions
=============================================================================*/

/*-----------------------------------------------------------------------------
    FIFO Write
-----------------------------------------------------------------------------*/

void fifo_write(uint8_t byte){ 
    if(((tx_ring_buffer.head + 1) % MAX_SERIAL_BUFFER_SIZE) == tx_ring_buffer.tail){
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "FIFO", "FIFO tx buffer overflow tx byte dropped. Byte: ", byte);
    }
    else{
        // Queue a byte for transmission.
        tx_ring_buffer.buffer[tx_ring_buffer.head] = byte;  
        tx_ring_buffer.head = (tx_ring_buffer.head + 1) % MAX_SERIAL_BUFFER_SIZE;
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "byte written to the tx_ring_buffer. Byte: ", byte); 
    } 
}


/*-----------------------------------------------------------------------------
    FIFO Available 
-----------------------------------------------------------------------------*/

uint8_t fifo_available(){
  uint8_t available = (rx_ring_buffer.head + MAX_SERIAL_BUFFER_SIZE - rx_ring_buffer.tail) % MAX_SERIAL_BUFFER_SIZE;
  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "data available to read. available: ", available);
  return available; 
}


/*-----------------------------------------------------------------------------
    FIFO Read
-----------------------------------------------------------------------------*/

uint8_t fifo_read(){
    if(rx_ring_buffer.tail == rx_ring_buffer.head){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "FIFO", "No data in rx_ring_buffer to read");
    }
    else{ 
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "data read from rx_ring buffer: ", rx_ring_buffer.buffer[rx_ring_buffer.tail]);
        uint8_t byte = rx_ring_buffer.buffer[rx_ring_buffer.tail] ;
        rx_ring_buffer.tail = (rx_ring_buffer.tail +1) % MAX_SERIAL_BUFFER_SIZE;
        return byte;
    }
    return 0;
}


/*-----------------------------------------------------------------------------
    FIFO Begin 
-----------------------------------------------------------------------------*/

uint8_t fifo_begin(uint32_t baud_rate){
    outgoing_transmission_timer = (uint32_t)(((BITS_PER_FRAME * MICROS_PER_SECOND) / baud_rate) + 0.5f);
    DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_INFO, "FIFO", "outgoing_transmission_timer set to " , outgoing_transmission_timer, " us.");
    return 1; 
}

/*-----------------------------------------------------------------------------
    FIFO Update
-----------------------------------------------------------------------------*/

void fifo_update(){
    if(tx_ring_buffer.head != tx_ring_buffer.tail){

        // Cache the elapsed time to avoid multiple micros() calls.
        uint32_t elapsed_time = micros() - last_tx_engine_update; 

        uint8_t next_rx_head = (rx_ring_buffer.head + 1) % MAX_SERIAL_BUFFER_SIZE; 

        if(next_rx_head == rx_ring_buffer.tail){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "rx_ring_buffer overflow detected");
        }
        else{
            if(elapsed_time >= outgoing_transmission_timer){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "fifo uart engine updated");
                rx_ring_buffer.buffer[rx_ring_buffer.head] = tx_ring_buffer.buffer[tx_ring_buffer.tail];
                last_tx_engine_update += outgoing_transmission_timer; 
                rx_ring_buffer.head = (rx_ring_buffer.head + 1) % MAX_SERIAL_BUFFER_SIZE;
                tx_ring_buffer.tail = (tx_ring_buffer.tail + 1) % MAX_SERIAL_BUFFER_SIZE; 
            }
            else{
                DEBUG_PRINT_MSG_VAL_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "remaining time until next transmission: ", outgoing_transmission_timer - elapsed_time, "us.");
            }
        }
    }
    else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "FIFO", "no data available to transmit in the tx_buffer.");
    }
}


