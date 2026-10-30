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


/*=============================================================================*
 * Recursively list LittleFS directory contents
 *=============================================================================*/

static void list_directory_contents(File directory, uint8_t depth = 0) {

    File file = directory.openNextFile();

    while (file) {

        // Cache file information before moving to the next entry.
        const char* name = file.name();
        const size_t size = file.size();
        const bool isDirectory = file.isDirectory();

        // Indent according to directory depth.
        for (uint8_t i = 0; i < depth; i++) {
            DEBUG_PORT.print("    ");
        }

        DEBUG_PORT.printf("%-24s | %8u bytes | %-9s\n",
            name,
            static_cast<unsigned int>(size),
            isDirectory ? "Directory" : "File"
        );

        // Recursively list directory contents.
        if (isDirectory) {
            list_directory_contents(file, depth + 1);
        }

        file.close();

        file = directory.openNextFile();
    }
}


void filesystem_list_files() {

    File root = LittleFS.open("/");

    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" LittleFS Data");
    DEBUG_PORT.println("========================================");

    // Print filesystem status.
    DEBUG_PORT.print("Filesystem successfully mounted: ");
    DEBUG_PORT.println(filesystem_status() ? "True" : "False");

    // Validate root directory.
    if (!root || !root.isDirectory()) {
        DEBUG_PORT.println("Failed to open LittleFS root directory");
        return;
    }

    DEBUG_PORT.println("File System type: Directory");

    // Print table header.
    DEBUG_PORT.println("File name                | Size           | Type");
    DEBUG_PORT.println("-------------------------|----------------|----------");

    // List all files and directories.
    list_directory_contents(root);

    DEBUG_PORT.println();
    root.close();
}


// void filesystem_list_files(){

//     File root = LittleFS.open("/"); 
//     File file = root.openNextFile();

//     DEBUG_PORT.println("========================================");
//     DEBUG_PORT.println(" LittleFS Data");
//     DEBUG_PORT.println("========================================");

//     // print file ststem status
//     DEBUG_PORT.print("Filesystemm sucesfuly mounted: ");
//     DEBUG_PORT.println(filesystem_status() ? "True" : "False"); 

//     DEBUG_PORT.print("File System type: "); 

//     if(root.isDirectory()){
//         DEBUG_PORT.println("Directory");
    
//         // print table header if the file is a directory
//         DEBUG_PORT.println("File name        | Size           | Directory");
//         DEBUG_PORT.println("-----------------|----------------|----------");
        
//     }
//     else {

//         DEBUG_PORT.println("File"); 
//         return; 
//     }

    
//     while(file){

//         // cache stoage
//         const char* name = file.name(); 
//         const size_t size = file.size();
//         const bool directory = file.isDirectory(); 
    
//         //print_file_data(name, size, directory); creae in debug 

//         // print the formated data
//         DEBUG_PORT.printf("%-16s | %8u bytes | %-9s\n",name,size, directory ? "Directory" : "File");


//         file = root.openNextFile(); 
//     }

//     DEBUG_PORT.println();

//     root.close();
// }




