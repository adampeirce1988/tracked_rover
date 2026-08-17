#include "simulation.h"
#include "transport.h"

/*=============================================================================*
 * Simulation Runtime State
*=============================================================================*/

namespace simulation
{
    // FIFO simulation state.
    bool fifo_active = false;

    // FIFO simulation requested by the user interface.
    bool fifo_user_requested = false;

    // FIFO simulation requested by self-test.
    bool fifo_test_request = false;
}


/*=============================================================================*
 * FIFO Simulation Control
 *=============================================================================*/

bool fifo_simulation_requested(){
    return simulation::fifo_test_request || simulation::fifo_user_requested;
}

bool fifo_simulation_active(){
    return simulation::fifo_active;
}

void request_fifo_test_simulation(){
    simulation::fifo_test_request = true; 
}

void release_fifo_test_simulation(){
    simulation::fifo_test_request = false; 
}


/*=============================================================================*
 * User Simulation Control
 *=============================================================================*/

void user_enable_simulation(){  // call from the web interface only
    simulation::fifo_user_requested = true; 
}

void user_disable_simulation(){
    simulation::fifo_user_requested = false; 
}


/*=============================================================================*
 * Simulation Manager
 *=============================================================================*/

void run_simulations(){

    const bool requested_state = fifo_simulation_requested();

    if(requested_state != simulation::fifo_active){

        simulation::fifo_active = requested_state; 

       if(simulation::fifo_active){ 
            transport_set_fifo();
       }
       else{
            transport_set_uart();
       }
    }
    
    // run the fifo engine if fifo_active: true.
    if(simulation::fifo_active){ 
        fifo_io_uart_engine_update(); 
    }
}