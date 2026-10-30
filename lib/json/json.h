#ifndef JSON_H
#define JSON_H


#include <telemetry.h>
#include <ArduinoJson.h>


// function declerations 

String generate_telemetry_json_string(const telemetry_data_t& data);  //generates and returns a JSON string


#endif