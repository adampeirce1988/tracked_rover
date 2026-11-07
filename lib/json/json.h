#ifndef JSON_H
#define JSON_H


#include <telemetry.h>
#include <ArduinoJson.h>


// function declerations 

String generate_ls_telemetry_json_string(const low_speed_telemetry_data_t& data);  // generates and returns a JSON string
String generate_hs_telemetry_json_string(const high_speed_telemetry_data_t& data);  // generates and returns a JSON string


#endif