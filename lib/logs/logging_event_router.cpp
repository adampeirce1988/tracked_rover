#include <stdint.h>
#include "debug.h"
#include "transport.h"
#include "protocol.h"
#include "metrics.h"
#include "self_test_logger.h"
#include "runtime_error_log.h"
#include "global.h"
#include "logger.h"

#define DEBUG_FILE DBG_ST_LOG
 

void  process_transport_tx_return_error(uint8_t return_code){
   
    switch(return_code){

        case TX_RETURN_CODES::TX_TRANSMISION_SUCCESS:
            // store the latancy of the last frame
            st_log_event(ST_LOG_EVENT::EVENT_PACKET_SENT, LOG_TYPE::METRIC);
            tx_latency_add_value(transport_get_tx_latancy());
        break; 
        
        case TX_RETURN_CODES::TX_ACK_TRANSMISION_SUCCESS:
                st_log_event(ST_LOG_EVENT::EVENT_ACK_SENT, LOG_TYPE::METRIC);
        break; 

        case TX_RETURN_CODES::TX_RETRY_TRANSMISION_SUCCESS:
                st_log_event(ST_LOG_EVENT::EVENT_RETRY_SENT, LOG_TYPE::METRIC);
        break; 

        case TX_RETURN_CODES::ACK_NOT_RECEVIED:
                st_log_event(ST_LOG_EVENT::EVENT_ACK_NOT_RECEIVED, LOG_TYPE::ERROR); 
                rt_log_error(rt_error_log_array[ERROR_ID_ACK_NOT_RECEIVED], false);
        break; 

        case TX_RETURN_CODES::ACK_WDT_TIMEOUT:
            // log ack_wdt 
                st_log_event(ST_LOG_EVENT::EVENT_ACK_WDT_TIEOUT, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_ACK_WDT_TIMEOUT], true);
        break; 
    
        case TX_RETURN_CODES::ACK_MISMATCHED: 
                st_log_event(ST_LOG_EVENT::EVENT_ACK_MISMATCH, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_ACK_MISMATCH], false);
        break; 

        case TX_RETURN_CODES::TX_BUFFER_OVERFLOW:
            // log buffer overflow 
                st_log_event(ST_LOG_EVENT::EVENT_TX_BUFFER_OVERFLOW, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_TX_BUFFER_OVERFLOW], true);

        break; 

        default: 
            // unecpected return code received. 
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "LOG", "return code not mapped to a self test logging function", return_code); 
            break; 
    }
}


void process_transport_rx_return_error(uint8_t rx_return_code){

    switch(rx_return_code){
        case RX_RETURN_CODES::FRAME_READY:
            // store the latancy of the last received frame
            rx_latency_add_value(transport_get_rx_latancy());
            st_log_event(ST_LOG_EVENT::EVENT_PACKET_RECEIVED, LOG_TYPE::METRIC);
        break;
        
        case RX_RETURN_CODES::ACK_READY:
            rx_latency_add_value(transport_get_rx_latancy());
            st_log_event(ST_LOG_EVENT::EVENT_ACK_RECEIVED , LOG_TYPE::METRIC);
            rt_clear_latch(rt_error_log_array[ERROR_ID_ACK_WDT_TIMEOUT]); 
            // reset tx and rx ack related error flags 
        break; 

        case RX_RETURN_CODES::NACK_REQUEST_RECEIVED: 
            rx_latency_add_value(transport_get_rx_latancy());
            st_log_event(ST_LOG_EVENT::EVENT_NACK_REVEIVED, LOG_TYPE::METRIC);
            
        break; 
        
        case RX_RETURN_CODES::INVALID_TYPE:
            // this should be loged from protocol
            st_log_event(ST_LOG_EVENT::EVENT_INVALID_TYPE , LOG_TYPE::ERROR);
        break;

        case RX_RETURN_CODES::ACK_OUT_OF_RANGE:
            // log error 
                st_log_event(ST_LOG_EVENT::EVENT_ACK_OUT_OF_RANGE, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_ACK_OUT_OF_RANGE], false);
    
        break; 

        case RX_RETURN_CODES::DLC_OVER_CAPACITY:
            // log error 
                st_log_event(ST_LOG_EVENT::EVENT_DLC_OVER_CAPACITY , LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_DLC_OVER_CAPACITY], false);
        break;

        case RX_RETURN_CODES::PAYLOAD_OVERFLOW:
           // log error
                st_log_event(ST_LOG_EVENT::EVENT_PAYLOAD_OVERFLOW, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_PAYLOAD_OVERFLOW], false);
        break; 

        case RX_RETURN_CODES::CRC_ERROR:
            // log error
                st_log_event(ST_LOG_EVENT::EVENT_CRC_ERROR, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_CRC_ERROR], false);
        break; 

        case RX_RETURN_CODES::MSG_TIMEOUT_ERROR:
            // log error
                st_log_event(ST_LOG_EVENT::EVENT_MSG_TIMEOUT_ERROR, LOG_TYPE::ERROR);
                rt_log_error(rt_error_log_array[ERROR_ID_MSG_TIMEOUT], false);
       
        break;

        default:
            break; 
    }

}

void process_protocol_return_error(uint8_t protocol_return_code){

    switch(protocol_return_code){
        case PROTOCOL_RX_RETURN_CODE::PROTO_INVALID_TYPE:
        // log error her
        // log st here
        break; 

        case PROTOCOL_RX_RETURN_CODE::PROTO_RESERVED_TYPE:
        //nlog st here
        break; 

        case PROTOCOL_RX_RETURN_CODE::PROTO_VALID_MSG_INHIBITED: 
        //nlog st here 
        break; 

        case PROTOCOL_RX_RETURN_CODE::PROTO_VALID_TYPE:
        // log st here
        break; 

        default: 
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "LOG", "return code not mapped to a self test logging function", protocol_return_code); 
        break;
    }
}
