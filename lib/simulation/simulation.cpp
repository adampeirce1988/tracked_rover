
#include "transport.h"
#include "global_config.h"
#include "debug.h" // remove once fsm bug resolved 
#define DEBUG_FILE DBG_SYSTEM

// Simulation variables
namespace simulation{

// uart simulation (DEBUG & SELF_TEST)
bool fifo_active = false;          // current status
bool fifo_user_requested = false;  // requested via the user interface 
bool fifo_test_request = false;    // requested via self_test

};

// FIFO transport simulation control
bool fifo_simulation_request_active(){
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

void user_enable_simulation(){  // call from the web interface only
    simulation::fifo_user_requested = true; 
}

void user_disable_simulation(){
    simulation::fifo_user_requested = false; 
}



// simulation manager 
void run_simulations(){

    bool requested_state = fifo_simulation_request_active();

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