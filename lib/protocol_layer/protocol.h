#ifndef PROTOCOL_N
#define PROTOCOL_H

// ================= RX PROTOCOL RETURN CODES =================
namespace PROTOCOL_RX_RETURN_CODE {
    constexpr uint8_t PROTO_INVALID_TYPE = 0;
    constexpr uint8_t PROTO_VALID_TYPE = 1;
    constexpr uint8_t PROTO_VALID_MSG_INHIBITED = 2;  
    constexpr uint8_t PROTO_RESERVED_TYPE = 3;    
}

// ================= RX PROTOCOL RETURN CODES =================
namespace PROTOCOL_TX_RETURN_CODE { 

}
// ============================================================

//test move to diagnostics later. 

extern uint8_t missmatche_type_received; 

// ========================= API ==============================
uint8_t rx_message_task_dispatcher();
// ============================================================


// ================== function prototypes =====================
void establish_coms(); 
void confirm_coms();
// ============================================================

#endif
