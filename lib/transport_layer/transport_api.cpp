
#include "transport_internal.h"
#include "global_config.h"
#include "transport_types.h"


Transport_IO *current_transport = DEFAULT_TRANSPORT;   // communication port struct defined in transport.h and initialized in main.cpp


////////////////////////////////////////////
//          Transport io init()           //
////////////////////////////////////////////

void ports_init(uint32_t baud_rate){
  //current_transport->begin(baud_rate);
  uart_io.begin(baud_rate);
  fifo_io.begin(baud_rate);
}

////////////////////////////////////////////
//              set transport             //
////////////////////////////////////////////

bool transport_set_fifo(){
  return transport_set(&fifo_io);
}

bool transport_set_uart(){
  return transport_set(&uart_io);
}

bool transport_set_default(){
  return transport_set(DEFAULT_TRANSPORT);
}


////////////////////////////////////////////
//                   FIFO                 //
////////////////////////////////////////////

void fifo_io_uart_engine_update(){
   current_transport->update();
}

////////////////////////////////////////////
//                 Transport              //
////////////////////////////////////////////
