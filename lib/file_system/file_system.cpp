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

    DEBUG_PORT.println();
    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" LittleFS Data");
    DEBUG_PORT.println("========================================");

    // print file ststem status
    DEBUG_PORT.print("Filesystemm sucesfuly mounted: ");
    DEBUG_PORT.println(filesystem_status() ? "True" : "False"); 

    DEBUG_PORT.print("File System type: "); 

    if(root.isDirectory()){
        DEBUG_PORT.println("Directory");
    
        // print table header if the file is a directory
        DEBUG_PORT.println("File name        | Size           | Directory");
        DEBUG_PORT.println("-----------------|----------------|----------");
        
    }
    else {

        DEBUG_PORT.println("File"); 
        return; 
    }

    
    while(file){

        // cache stoage
        const char* name = file.name(); 
        const size_t size = file.size();
        const bool directory = file.isDirectory(); 
    
        //print_file_data(name, size, directory); creae in debug 

        // print the formated data
        DEBUG_PORT.printf("%-16s | %8u bytes | %-9s\n",name,size, directory ? "Directory" : "File");


        file = root.openNextFile(); 
    }

    DEBUG_PORT.println();

    root.close();
}