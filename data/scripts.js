
document.addEventListener("DOMContentLoaded", function(){

    // Debug: verify scripts.js is avaliable
    console.log("scripts.js loaded"); 


    //=============================================================================*
    // Connect HTML elements
    //=============================================================================*
    const firmwareFile = document.getElementById("firmware-file");
    const firmwareInfo = document.getElementById("firmware-info");
    const firmwareUploadButton = document.getElementById("firmware-upload-button"); 
    const vehicleFSMStatus = document.getElementById("vehicle-state");

    // Debug: verify HTML elements are available
    console.log(firmwareFile);
    console.log(firmwareInfo);
    console.log(firmwareUploadButton);
    console.log(vehicleFSMStatus); 

    //=============================================================================*
    // vehicel status menue
    //=============================================================================*

    function updateVehicleStatus (){

        fetch("/status")
            .then(response => response.json)
            .then(data => {
                vehicleFSMStatus.textContent = data.vehicle_state; 
        })
    }

    //=============================================================================*
    // Firmware upload
    //=============================================================================*
    firmwareUploadButton.addEventListener("click", function(){

        const file = firmwareFile.files[0];

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

    //=============================================================================*
    // Initial status update
    //=============================================================================*

    updateVehicleStatus();

});

