/*=============================================================================*
 * ESP-Nexus - Shared UI Scheduler
 *=============================================================================*/

console.log("scheduler.js loaded"); 

// Scheduler configuration
const UI_SCHEDULER_INTERVAL_MS = 100; 

// Scheduler state. 
let uiSchedulerInternal = null; 
const uiUpdateCallback = new Set();

// Register a function to run on each scheduler tick
function subscribeUIUpdate(callback){

    if(typeof callback !== "function"){

        console.error("UI scheduler subscriber is not a function"); 
        return function() {}; 
    }

    uiUpdateCallback.add(callback);

    // retunr the function that cancles the subscriber
    return function(){
        uiUpdateCallback.delete(callback); 
    };
}

// Run all regustered UI updates
function runUIUpdateCycle(){ 

    uiUpdateCallback.forEach(function(callback) { 

        try{
            callback();
        }
        catch(error){
            console.error("UI scheduler update failed: ", error); 
        }
    });
}

// Stat the scheduler if it is not already running
function startUIScheduler() {

    if(uiSchedulerInternal !== null) {
        return; 
    }

    uiSchedulerInternal = setInterval(runUIUpdateCycle, UI_SCHEDULER_INTERVAL_MS); 

    console.log("UI scheduler started"); 
}

function stopUIScheduler(){

    if(uiSchedulerInternal === null){
        return;
    }

    clearInterval(uiSchedulerInternal);
    uiSchedulerInternal = null;

    console.log("UI scheduler stoped");

}


window.UIScheduler = {
    subscribe: subscribeUIUpdate,
    start: startUIScheduler,
    stop: stopUIScheduler
}; 