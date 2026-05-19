#include<Arduino.h>
#include "system.h"
#include "protocol.h"
#include "global.h" // move sys:: these to the system folder. 
#include "debug.h"
#include "self_test.h"
#include "log.h"

#define DEBUG_FILE DBG_SYSTEM

bool request_vehicle_state_change(VEHICLE_STATE requested){

  switch(vehicle_state){

    case VEHICLE_STATE::SAFE_STATE:
      if(requested == VEHICLE_STATE::SAFE_STATE){
        sys::verbous_current_state = "SAFE_STATE";
      }
      else if(requested == VEHICLE_STATE::IDLE && health_test_status == true){
        vehicle_state = VEHICLE_STATE::IDLE;
        sys::verbous_current_state = "IDLE";
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::SAFE_STATE;
        vehicle_state = VEHICLE_STATE::DIAGNOSTICS;
        sys::verbous_current_state = "DIAGNOSTICS";
        diagnostics_WDT = millis();
      }
      else{
        // do nothing as vehicle still in safe stat
        return false;
      }
      return true; 
    break; 

    case VEHICLE_STATE::IDLE:
      if(requested == VEHICLE_STATE::SAFE_STATE){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::SAFE_STATE requested from IDLE.");
        vehicle_state = VEHICLE_STATE::SAFE_STATE; 
        sys::verbous_current_state = "SAFE_STATE";
      }
      else if(requested == VEHICLE_STATE::MANUAL){
        vehicle_state = VEHICLE_STATE::MANUAL;
        sys::verbous_current_state = "MANUAL"; 
      }
      else if(requested == VEHICLE_STATE::DIAGNOSTICS){
        return_state = VEHICLE_STATE::IDLE; 
        vehicle_state = VEHICLE_STATE::DIAGNOSTICS; 
        sys::verbous_current_state = "DAIGNOSTICS"; 
        diagnostics_WDT = millis();
      }
      else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::IDLE not avaliable form the current state");
        return false; 
      }
    return true;   
    break; 

      case VEHICLE_STATE::MANUAL:
        if(requested == VEHICLE_STATE::SAFE_STATE){
          vehicle_state = VEHICLE_STATE::SAFE_STATE;
          sys::verbous_current_state = "SAFE_STATE";
        }
        else if(requested == VEHICLE_STATE::IDLE){
          vehicle_state = VEHICLE_STATE::IDLE; 
          sys::verbous_current_state = "IDLE"; 
        }
        else{
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::MANUALE not avaliable form the current state");
          return false; 
        }
        return true;
      break; 

      case VEHICLE_STATE::AUTONOMOUS:
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: VEHICLE_STATE::AUTONOMOUS currently unsupported");
        return false; 
      break; 

      case VEHICLE_STATE::DIAGNOSTICS:  // this need looking at as its not possible return a state.

        if(requested == VEHICLE_STATE::SAFE_STATE && return_state == VEHICLE_STATE::SAFE_STATE){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN",  "DIAGNOSTICS returnung to VEHICLE_STATE::SAFE_STATE.");
          vehicle_state = return_state; 
          return_state = VEHICLE_STATE::SAFE_STATE;  
        }
        else if(requested == VEHICLE_STATE::IDLE && return_state == VEHICLE_STATE::IDLE){
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "MAIN",  "DIAGNOSTICS returnung to VEHICLE_STATE::IDLE.");
          vehicle_state = return_state; 
          return_state = VEHICLE_STATE::SAFE_STATE; 
        
        }
        else{
          DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "ERROR: unable to request current state transition form diagnostics.");
          return false; 
        }
        return true; 
      break; 

      case VEHICLE_STATE::FAIL_SAFE:
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "MAIN", "VEHICLE_STATE::FAIL_SAFE activated"); 

        // reset all valuse here hen return to safe
        // continue to opperte no commands to be actiones. 

        return false; 
      break; 

    break;
  }
  return false; 
}
