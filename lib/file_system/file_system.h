#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


bool filesystem_init();        // mount the file system returns false if init fails 
bool filesystem_status();      // get retuen true if the system is mounted
void filesystem_list_files();  // Debug list files in littleFS stored on the ESP32 


#endif