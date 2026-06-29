#ifndef PROTOCOL_N
#define PROTOCOL_H

// ================= RX PROTOCOL RETURN CODES =================
namespace PROTOCOL_RX_RETURN_CODE {
    constexpr uint8_t PROTO_IDLE = 0;
    constexpr uint8_t PROTO_INVALID_TYPE = 1;
    constexpr uint8_t PROTO_VALID_TYPE = 2;
    constexpr uint8_t PROTO_VALID_MSG_INHIBITED = 3;  
    constexpr uint8_t PROTO_RESERVED_TYPE = 4;    
}

// ================= RX PROTOCOL RETURN CODES =================
namespace PROTOCOL_TX_RETURN_CODE { 
// not yet implimented 
}
// ============================================================


// ========================= API ==============================
uint8_t rx_message_task_dispatcher();
// ============================================================


// ================== function prototypes =====================
void establish_coms(); 
void confirm_coms();
// ============================================================

#endif
