/* 
    Author: António Malato
    This is a c file for the web page management function of the tractor project
    It has the implementation of the web page management thread.
    This webpage will be used to communicate with the tractor changes in its settings, and show some status information

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    
*/

#include "common.h"
#include "web_page_management_task.h"

static const char *TAG = "WEBPAGE_TASK";


void webpage_task_start(void){

TaskHandle_t webpage_task_handle;

xTaskCreate(webpage_task, "Webpage Task", 8192, NULL, 5, &webpage_task_handle);
ESP_LOGI(TAG, "Webpage task created");

}

void webpage_task(void* pvParameters){
    ESP_LOGI(TAG, "Webpage task started");

    while(1){
        // Here the functions of the web page management will be used to communicate with the tractor changes in its settings, and show some status information
        // For example, read the settings from a global variable or a queue and update the web page accordingly
        //ESP_LOGI(TAG, "Webpage task running");
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms to avoid busy waiting

    }
}

