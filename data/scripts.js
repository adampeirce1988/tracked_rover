document.addEventListener("DOMContentLoaded", function(){

    console.log("scripts.js loaded"); 

    const firmwareFile = document.getElementById("firmware-file");
    const firmwareInfo = document.getElementById("firmware-info");
    const firmwareUploadButton = document.getElementById("firmware-upload-button"); 

    console.log(firmwareFile);
    console.log(firmwareInfo);
    console.log(firmwareUploadButton);

    firmwareUploadButton.addEventListener("click", function(){

        const file = firmwareFile.files[0];

        if(!file){

            firmwareInfo.textContent ="No Firmware file selcted"; 
        }

        console.log("Starting firmware upload");
        console.log(file.name);
        console.log(file.size);

        const formData = new FormData(); 
        formData.append("firmware", file);

        const xhr = new XMLHttpRequest();

        xhr.open("POST", "/upload-firmware", true);

        xhr.onload = function(){
            console.log("Upload response", xhr.status);
            console.log(xhr,this.responseText); 
        };

        xhr.onerror = function(){
            console.log("ERROR: upload filed"); 
        };

        xhr.send(formData);


    });
});
