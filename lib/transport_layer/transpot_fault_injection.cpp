
#include "transport.h"
#include "debug.h"

/*=============================================================================
    Debug Configuration
=============================================================================*/

#define DEBUG_FILE DBG_TRANSPORT

/*=============================================================================
    Enum Classes & structs 
=============================================================================*/
enum class TX_FAILURE_MODE : uint8_t{
    NONE,
    TYPE_CHANGE,
    ACK_CHANGE,
    ID_CHANGE,
    DLC_CHANGE,
    DLC_OVER_MAX_CAPACITY,
    CRC_RAND_FLIP_BIT, 
    CRC_CHANGE, 
    RAND_DATA_FLIP_BIT,
    RAND_DATA_CHANGE
};

struct TX_FAULT_INJECTION_CFG{
    TX_FAILURE_MODE mode = TX_FAILURE_MODE::NONE; 
    bool tx_fault_injection_flag = false; 
    uint8_t value = 0;
};


/*=============================================================================*
    TX Fault Injection Configuration
*=============================================================================*/

TX_FAULT_INJECTION_CFG tx_fault_injection_cfg; 


/*=============================================================================
    Function Prototypes 
=============================================================================*/

TX_FAILURE_MODE set_test_type(TX_FAULT_MODE type);


/*=============================================================================*
    Fault Injection Functions
*=============================================================================*/

void set_tx_fault_injection_active(TX_FAULT_MODE type, uint8_t value){
  tx_fault_injection_cfg.mode = set_test_type(type);
  tx_fault_injection_cfg.value = value; 
  tx_fault_injection_cfg.tx_fault_injection_flag = true; 
  DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "INJT", "tx_fault_injection_flag: ", tx_fault_injection_cfg.tx_fault_injection_flag); 
}


/*=============================================================================
    Error Type Selector FSM 
=============================================================================*/

TX_FAILURE_MODE set_test_type(TX_FAULT_MODE type){
    switch(type){
        case TX_FAULT_MODE::TYPE_CHANGE:           return TX_FAILURE_MODE::TYPE_CHANGE; 
        case TX_FAULT_MODE::ACK_CHANGE:            return TX_FAILURE_MODE::ACK_CHANGE; 
        case TX_FAULT_MODE::ID_CHANGE:             return TX_FAILURE_MODE::ID_CHANGE; 
        case TX_FAULT_MODE::RAND_DLC_CHANGE:       return TX_FAILURE_MODE::DLC_CHANGE;
        case TX_FAULT_MODE::DLC_OVER_MAX_CAPACITY: return TX_FAILURE_MODE::DLC_OVER_MAX_CAPACITY;
        case TX_FAULT_MODE::CRC_RAND_FLIP_BIT:     return TX_FAILURE_MODE::CRC_RAND_FLIP_BIT; 
        case TX_FAULT_MODE::CRC_CHANGE:            return TX_FAILURE_MODE::CRC_CHANGE; 
        case TX_FAULT_MODE::RAND_DATA_FLIP_BIT:    return TX_FAILURE_MODE::RAND_DATA_FLIP_BIT;
        case TX_FAULT_MODE::RAND_DATA_CHANGE:      return TX_FAILURE_MODE::RAND_DATA_CHANGE;
        
        default: 
            return TX_FAILURE_MODE::NONE;
    }
}
        

/*=============================================================================
    Fault Injection Engine 
=============================================================================*/

void tx_frame_error_injection(struct frame *f ){
    if(f == nullptr){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "INJT", "pointer passed to function tx_frame_error_injection(): NULL");
        return; 
    }

    if(tx_fault_injection_cfg.tx_fault_injection_flag){

        const char* str_failure_type = "UNKNOWN"; 

        switch(tx_fault_injection_cfg.mode){
            case TX_FAILURE_MODE::TYPE_CHANGE:{
                f->TYPE = tx_fault_injection_cfg.value; 
                str_failure_type = "f->TYPE";
                break;
            }

            case TX_FAILURE_MODE::ACK_CHANGE:
                f->ACK = tx_fault_injection_cfg.value; 
                str_failure_type = "f->ACK";
            break;

            case TX_FAILURE_MODE::ID_CHANGE:
                f->ID = tx_fault_injection_cfg.value; 
                str_failure_type = "f->ID";
            break;

            case TX_FAILURE_MODE::DLC_CHANGE:{
                uint8_t rand_dlc = random(0, MAX_PAYLOAD_LEN + 1); // random change
                if(rand_dlc == f->DLC){
                    if(f->DLC > 0){ 
                        f->DLC -= 1; 
                    }
                    else{
                        f->DLC += 1; 
                    }
                }
                else{
                    f->DLC = rand_dlc; 
                }

                str_failure_type = "f->DLC";
                
            } break;
            
            case TX_FAILURE_MODE::DLC_OVER_MAX_CAPACITY:{ 
                f->DLC = MAX_PAYLOAD_LEN + 1; 
                str_failure_type = "f->DLC";
            } break;

            case TX_FAILURE_MODE::CRC_RAND_FLIP_BIT:{
                // get random bit
                uint8_t bit_position = random(0, 8);  // random no between 0-7
                f->CRC ^= (1<<bit_position); 
                str_failure_type = "CRC_RAND_FLIP_BIT";
                break;
            }

            case TX_FAILURE_MODE::CRC_CHANGE:
            {
                f->CRC = tx_fault_injection_cfg.value;  
                str_failure_type = "f->CRC"; 
            }
            break;

            case TX_FAILURE_MODE::RAND_DATA_FLIP_BIT:
            { 
                // get the random position 
                uint8_t data_position = random(0, f->DLC); 
                uint8_t bit_position = random(0, 8); 
                f->payload[data_position] ^= (1<<bit_position); 
                str_failure_type = "RAND_DATA_FLIP_BIT";
            }    
            break;

            case TX_FAILURE_MODE::RAND_DATA_CHANGE:
            {
                uint8_t data_position = random(0, f->DLC); 
                uint8_t rand_data = random(0, 256); 

                if(f->payload[data_position] == rand_data){
                    rand_data ++; 
                }
                
                f->payload[data_position] = rand_data;

                str_failure_type = "RAND_DATA_CHANGE";
            }
            break;

            case TX_FAILURE_MODE::NONE: 
                str_failure_type = "NONE";
            break;
        }
        
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_TEST, "INJT", "TX fault injected type: ", str_failure_type); 
        tx_fault_injection_cfg.tx_fault_injection_flag = false; // must be cleared at the end of the call.
    }
    else{
        return; 
    }
}