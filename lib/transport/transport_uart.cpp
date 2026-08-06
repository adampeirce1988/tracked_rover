
#include <Arduino.h>
#include "transport.h"
#include "transport_internal.h"
#include "debug.h"

/*=============================================================================
    Debug Configuration
=============================================================================*/

#define DEBUG_FILE DBG_TRANSPORT_UART

/*=============================================================================
    Function Prototypes
=============================================================================*/

void    uart_write(uint8_t byte);
uint8_t uart_read();
uint8_t uart_available();
uint8_t uart_begin(uint32_t baud_rate);
void    uart_update();

/*=============================================================================
   UART Transport Interface
=============================================================================*/

const Transport_IO uart_io = {
    uart_write,
    uart_available,
    uart_read,
    uart_begin,
    uart_update
};

/*=============================================================================
    UART Interface Functions
=============================================================================*/

/*-----------------------------------------------------------------------------
    UART Write
-----------------------------------------------------------------------------*/

void uart_write(uint8_t byte){
    COMS_PORT.write(byte);
}

/*-----------------------------------------------------------------------------
    UART Read
-----------------------------------------------------------------------------*/

uint8_t uart_read(){
    return COMS_PORT.read();
}

/*-----------------------------------------------------------------------------
    UART Available
-----------------------------------------------------------------------------*/

uint8_t uart_available(){
    return COMS_PORT.available();
}


/*-----------------------------------------------------------------------------
    UART Begin 
-----------------------------------------------------------------------------*/

uint8_t uart_begin(uint32_t baud_rate){
    COMS_PORT.begin(baud_rate);
    DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "UART", "UART initialised at: ", baud_rate);
    return 1; 
}


/*-----------------------------------------------------------------------------* 
* UART Update  
*
* This function should never be called and will generate a verbose error if
* called. The UART transport is interrupt/hardware driven and requires no 
* periodic servicing. 
*
-----------------------------------------------------------------------------*/

void uart_update(){

    // Intentionally empty.
    // The UART transport is interrupt/hardware driven and requires

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UART", "function call to uart_update() is invalid.");
}