#ifndef TRANSPORT_TYPES_H
#define TRANSPORT_TYPES_H

// folder containing all of enumm class to be included in the external.h so there is no need to include them in every file. 
#include <stdint.h>

//data frame structure
struct frame {
  uint8_t TYPE;
  uint8_t ACK;
  uint8_t ID;
  uint8_t DLC;
  uint8_t payload[MAX_PAYLOAD_LEN];
  uint8_t CRC;
};

//================= ACK TYPES =================
namespace TRANSPORT_ACK_TYPE{
  constexpr uint8_t NORMAL_FRAME =              0x00;
  constexpr uint8_t ACK_REQUEST  =              0x01;
  constexpr uint8_t ACK_RESPONSE =              0x02;
  constexpr uint8_t NACK =                      0x03;
};

// ================= RX RETURN CODES =================
enum class RX_RETURN_CODES : uint8_t{
  RX_STATE_IDLE,
  WAITING_FOR_START,
  START_RECEIVED,
  TYPE_RECEIVED,
  ACK_REQUEST_RECEIVED,
  ACK_RESPONSE_RECEIVED,
  NACK_REQUEST_RECEIVED,
  NORMAL_FRAME_RECEIVED,
  ACK_RESPONSE_SENT,
  ID_RECEIVED,
  DLC_RECEIVED,
  RECEIVING_DATA,
  PAYLOAD_COMPLETE,
  ACK_READY,
  INVALID_TYPE,
  ACK_OUT_OF_RANGE,
  DLC_OVER_CAPACITY,
  PAYLOAD_OVERFLOW,
  CRC_ERROR,
  MSG_TIMEOUT_ERROR,
  FRAME_READY,
  UNINITIALIZED
};

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
