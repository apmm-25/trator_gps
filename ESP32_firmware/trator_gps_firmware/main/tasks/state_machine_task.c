/* 
    Author: António Malato
    This is a c file for the state machine logic function of the tractor project
    It has the implementation of the state machine logic thread.
    Here the functions of the state machine will be used to always be managing the states of the tractor

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    
*/

#include "common.h"
#include "state_machine_task.h"

static const char *TAG = "STATE_MACHINE_TASK";

void state_machine_task_start(void){

    TaskHandle_t state_machine_task_handle;
    xTaskCreate(state_machine_task, "State Machine Task", 4096, NULL, 5, &state_machine_task_handle);
    ESP_LOGI(TAG, "State machine task created");
}

void state_machine_task(void* pvParameters){
    ESP_LOGI(TAG, "State machine task started");

    while(1){
        // Here the functions of the state machine will be used to always be managing the states of the tractor
        // For example, read the state machine data and update the state of the tractor in a global variable or a queue
        ESP_LOGI(TAG, "State machine task running");

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms to avoid busy waiting

    }
}