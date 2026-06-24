/* 
    Author: António Malato
    This is a c file for the localization logic function of the tractor project
    It has the implementation of the localization logic thread.
    Here the functions of the gps and other sensors will be used to always be calculating the position of the tractor

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    
*/

#include "common.h"
#include "localization_task.h"    

static const char *TAG = "LOCALIZATION_TASK";

void localization_task_start(void){

    TaskHandle_t localization_task_handle;
    xTaskCreate(localization_task, "Localization Task", 4096, NULL, 5, &localization_task_handle);
    ESP_LOGI(TAG, "Localization task created");
}

void localization_task(void* pvParameters){
    ESP_LOGI(TAG, "Localization task started");

    while(1){
        // Here the functions of the gps and other sensors will be used to always be calculating the position of the tractor
        // For example, read the gps data and update the position of the tractor in a global variable or a queue
        ESP_LOGI(TAG, "Localization task running");
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms to avoid busy waiting

    }
}