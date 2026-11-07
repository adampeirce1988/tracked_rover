

#include <ArduinoJson.h>

#include "telemetry_types.h"


// generates a telemetry json string
String generate_ls_telemetry_json_string(const low_speed_telemetry_data_t& data){

    JsonDocument doc;

    doc["type"] = "ls_telemetry";
    doc["vehicle_status"] = data.vehicle_state; 
    doc["battery_voltage"] = data.battery_voltage; 
    doc["wifi_mode"] = data.wifi_mode;
    doc["wifi_signal"] = data.wifi_signal;

    String json_output; 

    serializeJson(doc, json_output);

    return json_output; 
}


String generate_hs_telemetry_json_string(const high_speed_telemetry_data_t& data){ 

    JsonDocument doc; 

    doc["type"] = "hs_telemetry";  // Test Values
    doc["acc_x"] = "0.12";         // (m/s2) 
    doc["acc_y"] = "-0.48";        // (m/s2) 
    doc["acc_z"] = "9.72";         // (m/s2) 

    doc["gyro_x"] = "12.5";        // (°/s)
    doc["gyro_y"] = "-8.3";        // (°/s)
    doc["gyro_z"] = "32.7";        // (°/s)

    String json_output; 

    serializeJson(doc, json_output);

    return json_output;
}