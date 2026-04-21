
#include <Arduino.h>
#include "transport.h"
#include "debug.h"
#include "global_config.h"
#include <HardwareSerial.h>

#define DEBUG_FILE DBG_ESP_MAIN

// will be removed in after testing.
#define PACKET_DELAY 500
#define MSG_RATE     250

bool rand_data = false;

bool uart_type = 0; // used to control the update funtion of the fifo_io simulated uart. 

uint8_t type = 0x01; 
uint8_t ack = 0x01;
uint8_t dlc = 0x06;
uint8_t data[6] ={0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

uint8_t tx_status = 0; 
uint8_t rx_status = 0; 
uint32_t last_msg = 0; 

uint8_t packets = 0; 
uint8_t no_of_tests = 5;

struct frame protocol_frame;


void setup(){

  debug_port_begin(); // open the debug port
  delay(500);
  PRINT_VERSION_DATA(SW_VERSION, HARDWARE_VERSION, RELEASE_NOTES); 

  // set the current transport an baud rate. 
  uart_type = transport_set(&fifo_io);
  coms_port_begin(COMS_PORT_BAUD);
  
}

void loop() {
  if(uart_type == 1){
    fifo_io_uart_engine_update(); 
  }
  
  rx_status = read_data_frame();
  tx_status = send_data_frame();
  
  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "MAIN", "tx_status: ", tx_status);
  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "MAIN", "rx_status: ", rx_status);
  
  
  if(packets < no_of_tests){
    if(rand_data == true){
      // get random data for testing
      for(int i = 0; i < dlc; i++){
        data[i] =random(0,255);
      }
    }

    //check for frame ready
    if(rx_status == FRAME_READY){
      DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "Packet received" );
      transport_get_frame(&protocol_frame);
      DEBUG_PRINT_DATA_FRAME(DEBUG_FILE, DEBUG_MSG, RETURNED_FRAME, START_BYTE, "MAIN", protocol_frame);
      DEBUG_PORT.println("***************** NEW MSG ********************");
      packets ++; //increment to only send 5 packes this will reduce the amount od data to debug

      // if true stop the program. 
      if(packets == no_of_tests){
        while(1){}
      }
    }
    
    if(tx_status == TX_SUCCESS){
      delay(PACKET_DELAY);
    }
    if(tx_status == TX_IDLE_STATE && (millis() - last_msg) > MSG_RATE){
      
      //DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN", "Packet sent" );
      transport_pack_and_send_packet(type, ack, dlc, data); // normal frame. 
      last_msg = millis();
    }
  }
}


