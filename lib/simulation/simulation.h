#ifndef SIMULATION_H
#define SIMULATION_H


/*=============================================================================*
 * Simulation Manager
 *=============================================================================*/

// Run the simulation manager.
void run_simulations();


/*=============================================================================*
 * FIFO Simulation
*=============================================================================*/

// Return whether a FIFO simulation has been requested.
bool fifo_simulation_requested();

// Return whether FIFO simulation is currently active.
bool fifo_simulation_active();

// Request FIFO simulation for self-test operation.
void request_fifo_test_simulation();

// Release the FIFO simulation request.
void release_fifo_test_simulation();


/*=============================================================================*
 * User Simulation Control
 *=============================================================================*/

// Enable FIFO simulation by user request.
void user_enable_simulation();

// Disable FIFO simulation by user request.
void user_disable_simulation();


#endif