

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdint.h>

#include "global_config.h"
#include "transport_types.h"
#include "transport_structs.h"

/*=============================================================================
    Transport Layer - Public Interface

    This header exposes the public API for the transport layer.

    Responsibilities:
        - Initialise the transport interfaces.
        - Queue messages for transmission.
        - Retrieve received frames.
        - Update the transmit and receive state machines.
        - Configure diagnostic fault injection.
        - Provide transmission performance statistics.

    This header intentionally hides all internal transport state,
    buffers and implementation details.
=============================================================================*/


/*=============================================================================
    Initialisation
=============================================================================*/

/// Initialise the transport layer and all supported communication ports.
bool transport_init(uint32_t baud_rate);

/// Select the FIFO simulation transport.
bool transport_set_fifo();

/// Select the hardware UART transport.
bool transport_set_uart();

/// Restore the default transport interface.
bool transport_set_default();


/*=============================================================================
    Messaging API
=============================================================================*/

/// Queue a message for transmission.
void transport_queue_message(uint8_t type, uint8_t ack, uint8_t dlc, uint8_t *data);

/// Retrieve the most recently received frame. 
void transport_get_frame(frame *out);

/// Returns non-zero when a complete frame is available.
bool frame_available();


/*=============================================================================
    Simulation
=============================================================================*/

// Update the FIFO UART simulation engine.
void fifo_io_uart_engine_update();


/*=============================================================================
    Diagnostics
=============================================================================*/

// Configure transmit fault injection.
void set_tx_fault_injection_active(TX_FAULT_MODE type, uint8_t value);

// Returns the last measured transmit latency (µs).
uint16_t transport_get_tx_latancy();

// Returns the last measured receive latency (µs).
uint16_t transport_get_rx_latancy();


/*=============================================================================
    State Machines

    These functions must be called once per main loop in the
    following order:

        1. update_rx_fsm();
        2. update_tx_fsm();
=============================================================================*/

RX_RETURN_CODES update_rx_fsm();
TX_RETURN_CODES update_tx_fsm();

#endif