

/*=============================================================================*
 * Firmware Update
 *=============================================================================*/

document.addEventListener("DOMContentLoaded", function() {

    // load the main menu
    loadMainMenu();

    // Initialise firmware upload
    initialiseFirmwareUpload();

});


//=============================================================================*
// Firmware upload
//=============================================================================*
function initialiseFirmwareUpload(){

    const firmwareFile = document.getElementById("firmware-file");
    const firmwareFileName = document.getElementById("firmware-file-name")
    const firmwareUploadButton = document.getElementById("firmware-upload-button");

    // Debug: verify HTML elements are available
    console.log("Firmware file input:", firmwareFile);
    console.log("Filename display:", firmwareFileName);
    console.log("Upload button:", firmwareUploadButton);

    // Ensure this page contains the required elements
    if (!firmwareFile || !firmwareFileName || !firmwareUploadButton) {
        console.error("Firmware upload elements not found");
        return;
    }

    // Update displayed filename when a file is selected
    firmwareFile.addEventListener("change", function(){

        const file = firmwareFile.files[0];

        if(file){
            firmwareFileName.textContent = file.name;
        } else {
            firmwareFileName.textContent = "No file selected";
        }

    });

    firmwareUploadButton.addEventListener("click", function(){

        const file = firmwareFile.files[0];

        console.log("[OTA] Upload button clicked");

        // check a file exsists 
        if(!file){
            firmwareInfo.textContent ="No Firmware file selcted"; 
            console.log("No firmware file sellected");
            return; 
        }

        // Debug: verify firmware file information
        console.log("Starting firmware upload");
        console.log(file.name);
        console.log(file.size);

        // Create upload form
        const formData = new FormData(); 
        formData.append("firmware", file);

        // Debug verifys that the file size is stored correctly  
        console.log("Update URL:", `/update-firmware?size=${file.size}`);

        // Create HTTP request
        const xhr = new XMLHttpRequest();
        xhr.open("POST", `/update-firmware?size=${file.size}`, true);

        // Report firmware upload progress
        xhr.upload.onprogress = function(event){
            const percentage = Math.round((event.loaded / event.total) * 100); 
            
            const progressBar = document.getElementById("firmware-progress-bar");
            const progressText = document.getElementById("firmware-progress-text");

            progressBar.style.width = percentage + "%"; 
            progressText.textContent = percentage + "%"

            console.log("[OTA] Upload progress: ", percentage + "%"); 
        }

        // Handle server response
        xhr.onload = function(){
            console.log("Upload response: ", xhr.status);
            console.log("Server response: ", xhr.responseText); 
        };

        // Handle upload failure
        xhr.onerror = function(){
            console.log("ERROR: upload filed"); 
        };

        // Send firmware files
        xhr.send(formData);

    });

}


