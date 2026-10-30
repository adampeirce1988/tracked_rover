

#include <ArduinoJson.h>

#include "telemetry_types.h"


// generates a telemetry json string
String generate_telemetry_json_string(const telemetry_data_t& data){

    JsonDocument doc;

    doc["type"] = "telemetry";
    doc["vehicle_status"] = data.vehicle_state; 
    doc["battery_voltage"] = data.battery_voltage; 

    String json_output; 

    serializeJson(doc, json_output);

    return json_output; 
}

