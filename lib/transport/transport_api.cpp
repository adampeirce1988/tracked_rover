
#include "transport_internal.h"
#include "global_config.h"
#include "transport_types.h"

/*=============================================================================
    Runtime Objects
=============================================================================*/

Transport_IO *current_transport = DEFAULT_TRANSPORT;


/*=============================================================================
    Transport Initialisation
=============================================================================*/

void transport_init(uint32_t baud_rate){
  uart_io.begin(baud_rate);
  fifo_io.begin(baud_rate);
}

/*=============================================================================
    Transport Selection
=============================================================================*/

bool transport_set_fifo(){
  return transport_set(&fifo_io);
}

bool transport_set_uart(){
  return transport_set(&uart_io);
}

bool transport_set_default(){
  return transport_set(DEFAULT_TRANSPORT);
}


/*=============================================================================
    FIFO Simulation Interface
=============================================================================*/

void fifo_io_uart_engine_update(){
  current_transport->update();
}


/*=============================================================================
    Transport Status
=============================================================================*/

bool frame_available(){
  return rx_packet.frame_ready;
}
