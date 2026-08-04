#ifndef TRANSPORT_INTERNAL_H
#define TRANSPORT_INTERNAL_H

#include <stdint.h>

#include "transport_structs.h"
#include "transport_types.h"

/*=============================================================================
    Transport Layer - Internal Interface

    This header contains the private definitions used by the transport layer.

    Responsibilities:
        - Internal transport state machines
        - Packet runtime structures
        - Transport interface abstraction
        - Timing information
        - Internal helper functions

    This file must only be included by transport source files.
=============================================================================*/


/*=============================================================================
    Utility Macros
=============================================================================*/

#define ARRAY_SIZE(arr)        (sizeof(arr) / sizeof((arr)[0]))
#define CONVERT_US_TO_MS(us)   ((us) / 1000)


/*=============================================================================
    Transport Constants
=============================================================================*/

constexpr uint8_t START_BYTE = 0xFF;


/*=============================================================================
    Internal State Machines
=============================================================================*/

enum TX_STATE
{
    TX_STATE_IDLE,
    TX_STATE_SENDING,
    TX_STATE_RETRY,
    TX_STATE_SUCCESS,
    TX_STATE_FAILED
};

enum RX_STATE
{
    RX_STATE_WAIT_START,
    RX_STATE_READ_TYPE,
    RX_STATE_READ_ACK,
    RX_STATE_READ_ID,
    RX_STATE_READ_DLC,
    RX_STATE_READ_PAYLOAD,
    RX_STATE_READ_CRC,
    RX_STATE_ERROR
};


/*=============================================================================
    Internal Packet Structures
=============================================================================*/

struct transmit_packet
{
    frame f;

    bool waiting = false;

    TX_RETURN_CODES error_code = TX_RETURN_CODES::TX_IDLE_STATE;
};

struct transmit_ack_packet
{
    frame f;

    bool waiting = false;
    bool retry = false;
    bool failed = false;
    bool ack_confirmation = false;

    uint32_t ack_timestamp = 0;
    uint8_t retry_counter = 0;

    TX_RETURN_CODES error_code = TX_RETURN_CODES::UNINITIALIZED;
};

struct receive_packet
{
    frame f;

    uint8_t crc_check = 0;
    uint8_t payload_position = 0;

    bool ack_response = false;
    bool frame_ready = false;

    RX_RETURN_CODES error_code = RX_RETURN_CODES::UNINITIALIZED;
};

struct receive_ack
{
    uint8_t TYPE = 0x00;
    uint8_t ID   = 0x00;

    bool ack_valid = false;
};


/*=============================================================================
    Internal Runtime Objects
=============================================================================*/

// packet struct declerations 
extern struct transmit_packet tx_priority_packet;
extern struct transmit_packet tx_normal_packet; 
extern struct transmit_ack_packet tx_pending_ack;

extern struct receive_packet rx_packet; 
extern struct receive_ack rx_ack;
 
extern struct frame *active;

/*=============================================================================
    Transport Interface
=============================================================================*/

struct Transport_IO
{
    void    (*write)(uint8_t);
    uint8_t (*available)();
    uint8_t (*read)();
    uint8_t (*begin)(uint32_t);
    void    (*update)();
};

extern Transport_IO fifo_io;
extern Transport_IO uart_io;
extern Transport_IO *current_transport;


/*=============================================================================
    Variables
=============================================================================*/

/* Timing */
extern uint32_t tx_start_timestamp;
extern uint32_t rx_start_timestamp;

/* Statistics */
extern uint32_t total_tx_frame_time;
extern uint32_t total_rx_frame_time;

/* Watchdogs */
extern uint32_t last_read_byte;
extern uint32_t rx_wdt_timestamp;

/*=============================================================================
    Internal State
=============================================================================*/

extern uint8_t packet_id;

extern TX_STATE tx_state;
extern RX_STATE rx_state;


/*=============================================================================
    Internal Function Prototypes
=============================================================================*/

bool transport_set(Transport_IO *io);

uint8_t calculate_crc(frame *f);
uint8_t inline_crc_calc(uint8_t crc, uint8_t byte);

bool rx_msg_wdt_check();

void transmit_packet(frame *f);

bool pack_message( uint8_t type, uint8_t ack, uint8_t id, uint8_t dlc, uint8_t *data, frame *ptr);

void pack_ack( uint8_t type, uint8_t id, frame *f);

void copy_frame(frame *source, frame *destination);

void flush_struct(frame *f);

void clear_rx_ack(); 

void reset_rx_message_struct();

void tx_frame_error_injection(frame *f);

#endif