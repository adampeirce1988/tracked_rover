#ifndef TRANSPORT_INTERNAL_H
#define TRANSPORT_INTERNAL_H

#include <stdint.h>

constexpr uint8_t START_BYTE =                0xFF;

// port structure
struct Transport_IO {
  void (*write)( uint8_t );
  uint8_t (*available)();
  uint8_t (*read)();
  uint8_t (*begin)(uint32_t);
  void (*update)(void);
};

// port declerations 
extern Transport_IO fifo_io; 
extern Transport_IO uart_io; 
extern Transport_IO *current_transport;

// Function decleratios
bool transport_set(Transport_IO *io);


#endif