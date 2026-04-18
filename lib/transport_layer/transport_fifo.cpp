
#include <Arduino.h>
#include "transport_uart.h"
#include "transport.h"
#include "global_config.h"
#include "debug.h"
#include "debug_config.h"

// define statments 
#define DEBUG_FILE DBG_TRANSPORT_FIFO
#define BUFF_SIZE 64

// Forward declarations
void fifo_write(uint8_t byte);
uint8_t fifo_read();
uint8_t fifo_available();
void fifo_begin(uint32_t baud_rate);

//uart struct instance
Transport_IO fifo_io = {
    fifo_write,
    fifo_available,
    fifo_read,
    fifo_begin
};

// internal functino portotypes //
uint8_t internal_avaliable();

uint8_t tx_ring_buffer[BUFF_SIZE];  // Simulated UART Ring buffer
uint8_t head = 0;                   // Ring buffer head
uint8_t tail = 0;                   // Ring buffer tail 
uint32_t tx_delay = 0;              // simulate baud rate delat
uint32_t last_tx_read = 0;          // stores the micros() time stap of the last reasdd byte
 
// functions
void fifo_write(uint8_t byte){
    if(internal_avaliable() < (BUFF_SIZE - 1)){
        tx_ring_buffer[head] = byte;
        head = ((head + 1) % BUFF_SIZE);
        DEBUG_PORT.print("data avaliable ib fifo buffer using intern avaliable() avaliable: ");
        DEBUG_PORT.print(internal_avaliable());
        
    }
}

uint8_t fifo_read(){
    uint8_t byte = tx_ring_buffer[tail]; 
    tail = ((tail + 1) % BUFF_SIZE); 
    last_tx_read = micros();
    return byte;
}

uint8_t fifo_available(){
    DEBUG_PRINT_FIFO_VAR(head, tail, tx_delay, last_tx_read);
    if(micros() - last_tx_read > tx_delay){
        return(head - tail + BUFF_SIZE) % BUFF_SIZE;     
    }
    else{
        return 0; //simulate the next byte not yet arrived. 
    }
}

 void fifo_begin(uint32_t baud_rate){
    tx_delay = (10 *1000000UL) / baud_rate;
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "FIFO", "communication port started at: ", baud_rate);
}


uint8_t internal_avaliable(){
    return(head - tail + BUFF_SIZE) % BUFF_SIZE;     
}

