

#include "system_internal.h"

/*=============================================================================*
 * Vehicle State Change
*==============================================================================*

 * Handles requests to transition the vehicle between defined system states.
 * Validates each requested transition and returns the result of the request.
 
*=============================================================================*/

STATE_CHANGE_RETURN_CODE request_vehicle_state_change(VEHICLE_STATE requested){

  STATE_CHANGE_RETURN_CODE result = STATE_CHANGE_RETURN_CODE::APPROVED; 

  switch(active_vehicle_state){

    case VEHICLE_STATE::BOOTING:
      if(requested == VEHICLE_STATE::BOOTING){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED;
      }
      else if(requested == VEHICLE_STATE::SAFE_STATE){
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE;
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
    break; 

    case VEHICLE_STATE::SAFE_STATE:
      if(requested == VEHICLE_STATE::SAFE_STATE){
       result = STATE_CHANGE_RETURN_CODE::UNCHANGED;
      }
      else if(requested == VEHICLE_STATE::IDLE){
        if(health_test_status == true){ // this will change to a system check function call when the system grows. 
          active_vehicle_state = VEHICLE_STATE::IDLE;
        }
        else{
          result = STATE_CHANGE_RETURN_CODE::DENIED;
        }
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::SAFE_STATE;
        active_vehicle_state = VEHICLE_STATE::DIAGNOSTICS;
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
      
    break; 

    case VEHICLE_STATE::IDLE:
      if(requested == VEHICLE_STATE::IDLE){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED; 
      }
      else if(requested == VEHICLE_STATE::SAFE_STATE){
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE; 
      }
      else if(requested == VEHICLE_STATE::MANUAL){
        active_vehicle_state = VEHICLE_STATE::MANUAL;
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::IDLE; 
        active_vehicle_state = VEHICLE_STATE::DIAGNOSTICS; 
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }   
    break; 

    case VEHICLE_STATE::MANUAL:
      if(requested == VEHICLE_STATE::MANUAL){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED; 
      }
      else if(requested == VEHICLE_STATE::SAFE_STATE){
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE;
      }
      else if(requested == VEHICLE_STATE::IDLE){
        active_vehicle_state = VEHICLE_STATE::IDLE; 
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
    break; 

    case VEHICLE_STATE::AUTONOMOUS:
      result = STATE_CHANGE_RETURN_CODE::DENIED;  
    break; 

    case VEHICLE_STATE::DIAGNOSTICS:  // this need looking at as its not possible return a state.
      if(requested == VEHICLE_STATE::DIAGNOSTICS){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED;
      }
      else if(requested == VEHICLE_STATE::SAFE_STATE && return_state == VEHICLE_STATE::SAFE_STATE){
        active_vehicle_state = return_state; 
        return_state = VEHICLE_STATE::SAFE_STATE;  
      }
      else if(requested == VEHICLE_STATE::IDLE && return_state == VEHICLE_STATE::IDLE){
        active_vehicle_state = return_state; 
        return_state = VEHICLE_STATE::SAFE_STATE; 
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
    break; 

    case VEHICLE_STATE::UPDATE:
      if(requested == VEHICLE_STATE::UPDATE){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED; 
      }
      else if(requested == VEHICLE_STATE::SAFE_STATE){
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE; 
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        active_vehicle_state = VEHICLE_STATE::DIAGNOSTICS;
      }
      else if(requested == VEHICLE_STATE::IDLE){
        active_vehicle_state = VEHICLE_STATE::IDLE;
      }
      else if(requested == VEHICLE_STATE::SHUTTING_DOWN){
        active_vehicle_state = VEHICLE_STATE::SHUTTING_DOWN;
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
    break; 

    case VEHICLE_STATE::FAIL_SAFE:
      if(requested == VEHICLE_STATE::FAIL_SAFE){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED;
      }
      else if(requested == VEHICLE_STATE::SAFE_STATE){
        active_vehicle_state = VEHICLE_STATE::SAFE_STATE; 
      }
      else{ 
      // this state will make the vehicle safe and then transition to safe_state. 
      result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
    break; 

    case VEHICLE_STATE::SHUTTING_DOWN:
      // no state implimented currently
      // this case will be used to safley close all files, and place all micro coontrolers into low power mode. 
      //active_vehicle_state = VEHICLE_STATE::SHUTTING_DOWN; 
      if(requested == VEHICLE_STATE::SHUTTING_DOWN){
        result = STATE_CHANGE_RETURN_CODE::UNCHANGED;
      }
      else{
        result = STATE_CHANGE_RETURN_CODE::DENIED;
      }
    break; 

  }

  return result;
}
