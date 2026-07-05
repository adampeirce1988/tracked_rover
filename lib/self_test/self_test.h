#ifndef SELF_TEST_H
#define SELF_TEST_H

#pragma once

#include <Arduino.h>
#include "global_config.h"
#include "debug.h"

//=============== avaliable test cased ================
enum class TEST_ID : uint8_t{     
    RETURN_TO_IDLE = 0,              // no test in progress (default: returs the state macheine back to idle)
    TRANSPORT_PACKET_NORMAL,         // Run a good packet test with moderate timing 
    TRANSPORT_PACKET_STRESS,         // run a good packet test with tight timing and random reduced time between transmisios
    TRANSPORT_TYPE_CHANGE,           // change the type wil rsult in a crc error
    TRANSPORT_ACK_CHANGE,            // change ack the will result in a crc error
    TRANSPORT_ID_CHANGE,             // change the current packet id will result in a crc error
    TRANSPORT_DLC_CHANG,             // changes the DLC to a renadom valid uint8_t
    TRANSPORT_DLC_OVERFLOW,          // DLC over max capactity test expeced early catch 
    TRANSPORT_CRC_BIT_FLIP,          // 1 bit in the CRC fliped randomly changed on 1 packet in 10.
    TRANSPORT_CRC_RAND_CHANGE,       // good packet test with the CRC randomly changed on 1 packet in 10.  
    TRANSPORT_DATA_BIT_FLIP,         // random data bit fliped expected crc error
    TRANSPORT_DATA_BYTE_CHANGE,      // random data byte change expected crc error
    DIAGNOSTIC_WATCHDOG_TIMEOUT      // diagnostics WDT test timeout test 
};

//=============== test status codes ================
enum class TEST_RETURN_STATUS{
    NO_TEST_RUNNING, 
    TEST_RUNNING, 
    TEST_PASSED, 
    TEST_FAILED,
    TEST_COMPLETED
}; 
//================ ================ ================ 

constexpr uint8_t ST_WDT_EXTRA_TIME =           250;   // tune to stop timout errors 
constexpr uint8_t PROGRESS_BAR_COUNT =          25;
//=============== gloabal variables ================


//====================== API function calls  ======================
TEST_RETURN_STATUS run_test_case();        // test state machine needs to be called every loop during a test
bool watchdog_test_active();               // returns truwe if a wdt test is in progrss 
bool check_diagnostic_status();            // returns true if a test is in progress 
bool request_self_test(TEST_ID test_id);   // call from main to acivate a test must have state macheine running.  

#endif
