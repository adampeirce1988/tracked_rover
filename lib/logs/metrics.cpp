
#include <stdint.h> 
#include "metrics.h"

// NOTE: API calls to be exposed in logging.h

/////////////////////////////////////////////////
//               define structs                //
/////////////////////////////////////////////////
latency_buffer rx_latency_buffer = {}; 
latency_buffer tx_latency_buffer = {}; 


/////////////////////////////////////////////////
//             internal dcelerations           //
/////////////////////////////////////////////////
static void latency_add_value(latency_buffer &b, uint16_t latency);
static void latency_clear_buffer(latency_buffer &b); 
static uint16_t latency_get_min(const latency_buffer &b);
static uint16_t latency_get_max(const latency_buffer &b);
static uint16_t latency_get_average(const latency_buffer &b);
static uint16_t latency_get_jitter(const latency_buffer &b);


//////////////////////////////////////////////////////////////////////////////////
//                           INTERNAL FUNCTIONS                                 //
//////////////////////////////////////////////////////////////////////////////////

static void latency_add_value(latency_buffer &b, uint16_t latency){
    // remove the value to be replaced from the total sum if the buffer is full
    if(b.count == LATENCY_BUFFER_SIZE){
        b.sum -= b.samples[b.index];    
    }
    else{ 
        b.count++; 
    }

    b.samples[b.index] = latency; 
    b.sum += latency; 

    b.index = (b.index + 1) % LATENCY_BUFFER_SIZE;
}


void latency_clear_buffer(latency_buffer &b){
    
    // the buffer can stay in its current state as all checks are done using b.coun
    b.count = 0; 
    b.index= 0; 
    b.sum = 0; 
}


static uint16_t latency_get_max(const latency_buffer &b){

    uint16_t max = 0; 

    if(b.count == 0){
        return 0;
    }

    for(uint16_t i = 0; i < b.count; i++){
        if(b.samples[i] > max){
            max = b.samples[i]; 
        }
    }

    return max; 
}


static uint16_t latency_get_min(const latency_buffer &b){

    uint16_t min = UINT16_MAX;

    if(b.count == 0){
        return 0;
    }

    for(uint16_t i = 0; i < b.count; i++){
        if(b.samples[i] < min){
            min = b.samples[i]; 
        }
    } 

    return min; 
}


static uint16_t latency_get_average(const latency_buffer &b){
    // protect against 0 division.
    if(b.count == 0){
        return 0;
    }
    
    return b.sum / b.count;
    
}

// TODO:
// Current implementation returns latency range (max - min).
// If latency stability becomes important, consider replacing
// with average deviation or standard deviation.
static uint16_t latency_get_jitter(const latency_buffer &b){

    return (latency_get_max(b) - latency_get_min(b));

}



//////////////////////////////////////////////////////////////////////////////////
//                             API FUNCTIONS.                                   //
//////////////////////////////////////////////////////////////////////////////////


// TX API functions 
void tx_latency_add_value(uint16_t latency){
    latency_add_value(tx_latency_buffer, latency);
}
void tx_latency_clear_buffer(){
    latency_clear_buffer(tx_latency_buffer);
}
uint16_t tx_latency_get_min(){
    return latency_get_min(tx_latency_buffer);
}
uint16_t tx_latency_get_max(){
    return latency_get_max(tx_latency_buffer);
}
uint16_t tx_latency_get_average(){
    return latency_get_average(tx_latency_buffer);

}
uint16_t tx_latency_get_jitter(){
    return latency_get_jitter(tx_latency_buffer);
}

// RX API functions 
void rx_latency_add_value(uint16_t latency){
    latency_add_value(rx_latency_buffer, latency);
}
void rx_latency_clear_buffer(){
    latency_clear_buffer(rx_latency_buffer);
}
uint16_t rx_latency_get_min(){
    return latency_get_min(rx_latency_buffer);
}
uint16_t rx_latency_get_max(){
    return latency_get_max(rx_latency_buffer);
}
uint16_t rx_latency_get_average(){
    return latency_get_average(rx_latency_buffer);
}
uint16_t rx_latency_get_jitter(){
    return latency_get_jitter(rx_latency_buffer);
}




