//#include <FS.h>
//#include <Arduino.h>
#include <LittleFS.h> 

#include "debug.h" // move the print funcion to here.


bool filesystem_mounted = false;  // must be initilised to false

bool filesystem_init(){
    
    if(LittleFS.begin()){
        filesystem_mounted = true; 
        DEBUG_PORT.println("Filesystem_mounted sucessfuly");  
    }
    else{
        DEBUG_PORT.println("Filesystem_mounted unsucesful");  
    }

    return filesystem_mounted;
}

bool filesystem_status(){
    return filesystem_mounted; 
}

void filesystem_list_files(){

    File root = LittleFS.open("/"); 
    File file = root.openNextFile();

    // print file ststem status
    DEBUG_PORT.print("Filesystemm status: ");
    DEBUG_PORT.println(filesystem_status() ? "True" : "False"); 

    if(!root.isDirectory()){
        DEBUG_PORT.println("File systen is not a directory");
        return; 
    }
    else {
        DEBUG_PORT.println("File system directory found."); 
    }

    while(file){

        // cache stoage
        const char* name = file.name(); 
        const size_t size = file.size();
        const bool directory = file.isDirectory(); 
    
        //print_file_data(name, size, directory); creae in debug 

        // temp solution. 
        DEBUG_PORT.print("File name: "); 
        DEBUG_PORT.print(name);
        DEBUG_PORT.print("   Size: ");
        DEBUG_PORT.print(size); 
        DEBUG_PORT.print("bytes   directory: ");
        DEBUG_PORT.println(directory ? "True" : "Fasle");

        file = root.openNextFile(); 
    }

    root.close();
}