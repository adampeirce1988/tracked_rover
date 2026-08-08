#ifndef TRANSPORT_STRUCTS_H
#define TRANSPORT_STRUCTS_H

#include "global_config.h"

/*=============================================================================
    Transport Layer - Frame Structures

    Defines the data structures used by the transport layer.

    Responsibilities:
        - Defines the transport frame format.
        - Provides shared frame structures between TX and RX modules.

=============================================================================*/

/*=============================================================================
    Transport Frame Structure
=============================================================================*/

struct frame {
  uint8_t TYPE;
  uint8_t ACK;
  uint8_t ID;
  uint8_t DLC;
  uint8_t payload[MAX_PAYLOAD_LEN];
  uint8_t CRC;
};

#endif