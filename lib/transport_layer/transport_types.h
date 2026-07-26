#ifndef TRANSPORT_INTERNAL_H
#define TRANSPORT_INTERNAL_H

// folder containing all of enumm class to be included in the external.h so there is no need to include them in every file. 
#include <stdint.h>

// // ================= ACK TYPES =================
// enum class TRANSPORT_ACK_TYPE : uint8_t{
// NORMAL_FRAME, 
// ACK_REQUEST,
// ACK_RESPONSE,
// NACK
// };

// // ================= RX RETURN CODES =================
// enum class RX_RETURN_CODES : uint8_t{
//   RX_STATE_IDLE,
//   WAITING_FOR_START,
//   START_RECEIVED,
//   TYPE_RECEIVED,
//   ACK_REQUEST_RECEIVED,
//   ACK_RESPONSE_RECEIVED,
//   NACK_REQUEST_RECEIVED,
//   NORMAL_FRAME_RECEIVED,
//   ACK_RESPONSE_SENT,
//   ID_RECEIVED,
//   DLC_RECEIVED,
//   RECEIVING_DATA,
//   PAYLOAD_COMPLETE,
//   ACK_READY,
//   INVALID_TYPE,
//   ACK_OUT_OF_RANGE,
//   DLC_OVER_CAPACITY,
//   PAYLOAD_OVERFLOW,
//   CRC_ERROR,
//   MSG_TIMEOUT_ERROR
// };

// ================= TX RETURN CODES =================
enum class TX_RETURN_CODES : uint8_t{
  TX_IDLE_STATE,
  TX_PENDING_ACK,
  TX_TRANSMITING,
  RESENDING_MSG,
  TX_TRANSMISION_SUCCESS,
  TX_ACK_TRANSMISION_SUCCESS,
  TX_RETRY_TRANSMISION_SUCCESS,
  TX_TRANSMISION_ERROR,
  ACK_NOT_RECEVIED,
  ACK_MISMATCHED,
  ACK_WDT_TIMEOUT,
  TX_BUFFER_OVERFLOW, 
  UNINITIALIZED
};

// =============== ENUM CLASS ===============
// global class used to select a test. 
enum class TX_SET_FAULT_MODE{           
  NONE,
  TYPE_CHANGE,
  ACK_CHANGE,
  ID_CHANGE,
  RAND_DLC_CHANGE,
  DLC_OVER_MAX_CAPACITY,
  CRC_RAND_FLIP_BIT, 
  CRC_CHANGE, 
  RAND_DATA_FLIP_BIT,
  RAND_DATA_CHANGE
}; 


#endif 
