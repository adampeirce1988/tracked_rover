#include <Arduino.h>
#include <stdint.h>
#include "self_test_internal.h"


bool request_self_test(uint8_t test_id){
    self_test::requested_test_id = test_id; 
    return true; 
}

bool analise_test_results(uint8_t expected_resutlts[] ){ //******* NEEDS TO BE COMPLETED *******
    if(true){
        return true; 
    }
    else{
        return false;
    }
}