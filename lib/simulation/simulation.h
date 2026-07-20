#ifndef SIMULATION_H
#define SIMULATION_H

// simulation egnine manager
void run_simulations(); 

// fifo simulation 
bool fifo_simulation_request_active();
bool fifo_simulation_active();
void request_fifo_test_simulation();
void release_fifo_test_simulation();

// fifo user request 
void user_enable_simulation();
void user_disable_simulation(); 

#endif