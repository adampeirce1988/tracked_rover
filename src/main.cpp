
#include <Arduino.h>
#include "transport.h"
#include "debug.h"
#include "global_config.h"
#include <HardwareSerial.h>

#define DEBUG_FILE DBG_ESP_MAIN

// will be removed in after testing.
#define PACKET_DELAY 500
#define MSG_RATE     250

bool rand_data = true;

uint8_t type = 0x01; 
uint8_t ack = 0x00;
uint8_t dlc = 0x06;
uint8_t data[6] ={0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

uint8_t tx_status = 0; 
uint8_t rx_status = 0; 
uint32_t last_msg = 0; 

struct frame protocol_frame;

void version_data();

void setup(){

  debug_port_begin(); // open the debug port
  
  delay(5000); // delay for a while to let the debug port initialize and print the version info before starting the main loop.
  coms_port_begin(COMS_PORT_BAUD);
  version_data();
  
}

void loop() {

  
  rx_status = read_data_frame();
  tx_status = send_data_frame();
  

  //DEBUG_PORT.println("");
  //PRINT_RUNTIME(millis());
  //DEBUG_PORT.print(get_ava());

  //Serial.print("tx_status: ");  Serial.println(tx_status);
  //Serial.print("rx_status: ");  Serial.println(rx_status);
  //Serial.print("fifo ava: ");  Serial.println(get_ava());
  
  

  if(rand_data == true){
    // get random data for testing
    for(int i = 0; i < dlc; i++){
      data[i] =random(0,255);
    }
  }

  //check for frame ready
  if(rx_status == FRAME_READY){
    transport_get_frame(&protocol_frame);
  }
  
  if(tx_status == TX_SUCCESS){
    delay(PACKET_DELAY);
  }
  if(tx_status == TX_IDLE_STATE && (millis() - last_msg) > MSG_RATE){
    Serial1.print("msg_sent");
    transport_send_packet(type, ack, dlc, data);
    last_msg = millis();
  
  }

}


void version_data(){
  #if TARGET_PLATFORM == ESP32
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "SYSTEM", "VERSION:", VERSION_ESP32);
  #elif TARGET_PLATFORM == MEGA2650
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "SYST", "VERSION:", VERSION_MEGA);
  #endif
}