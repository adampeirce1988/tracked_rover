#ifndef PROTOCOL_TYPES_H
#define PROTOCOL_TYPES_H

#include <stdint.h>

// ================= RX PROTOCOL RETURN CODES =================

enum class PROTOCOL_RX_RETURN_CODE : uint8_t{
     PROTO_IDLE,
     PROTO_INVALID_TYPE,
     PROTO_VALID_TYPE,
     PROTO_VALID_MSG_INHIBITED, 
     PROTO_RESERVED_TYPE
};

// ================= TX PROTOCOL RETURN CODES =================

enum class PROTOCOL_TX_RETURN_CODE : uint8_t{ 
// not yet implimented 
};

// ============================================================


#endif