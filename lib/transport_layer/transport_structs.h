#ifndef TRANSPORT_STRUCTS_H
#define TRANSPORT_STRUCTS_H

#include "global_config.h"

//data frame structure
struct frame {
  uint8_t TYPE;
  uint8_t ACK;
  uint8_t ID;
  uint8_t DLC;
  uint8_t payload[MAX_PAYLOAD_LEN];
  uint8_t CRC;
};

#endif