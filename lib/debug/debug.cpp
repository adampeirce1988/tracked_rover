
#include <Arduino.h>
#include "global_config.h"
#include "debug.h"
#include "debug_config.h"

#define DEBUG_FILE DBG_DEBUG 

// ================= FUNCTION PROTOTYPES =================
void debug_port_begin();


// ================= VARIABLES =================
bool verbous_debug_enabled = true; // used to disable verbous debugging during self tests. 

// ================= FUNCTIONS =================

void debug_port_begin(){
  #if TARGET_PLATFORM == ESP32
    DEBUG_PORT.begin(DEBUG_PORT_BAUD, MSG_FORMAT, DEBUG_PORT_RX_PIN, DEBUG_PORT_TX_PIN);
    
  #elif TARGET_PLATFORM == MEGA2650
    DEBUG_PORT.begin(DEBUG_PORT_BAUD);
  #else
    #error "TARGET_PLATFORM not defined or not supported. Please define TARGET_PLATFORM as either 'ESP32' or 'MEGA'."
  #endif

  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "PORT", "communication port stated at: ", DEBUG_PORT_BAUD);
}

void disable_verbose_error(){
  verbous_debug_enabled = false; 

}

void enable_verbose_error(){
  verbous_debug_enabled = true; 
}