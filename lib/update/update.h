#ifndef UPDATE_H
#define UPDATE_H


// enum for update selection
enum class UPDATE_TYPE : uint8_t{
    NONE,
    FIRMWARE, 
    FILESYSTEM 
};

// diagnostic / debig calls 
void ota_print_update_partition();
void ota_print_patrition_data();


// carry out an ota update
void run_update();


bool update_begin(UPDATE_TYPE type, size_t update_size);
bool update_write(const uint8_t* data, size_t length);
bool update_finalise(); 
bool update_abort(); 

#endif 