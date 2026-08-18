/* 
    Author: António Malato
    This is a c file for the web page management function of the tractor project
    It has the implementation of the web page management thread.
    This webpage will be used to communicate with the tractor changes in its settings, and show some status information

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    05/7/2026 - Added the start webserver call
    
*/

#include "common.h"
#include "web_page_management_task.h"

static const char *TAG = "WEBPAGE_TASK";
web_cmds_t web_page_received_settings_data_snapshot;
web_data_t web_data_to_send_snapshot;


void webpage_task_start(void){

TaskHandle_t webpage_task_handle;

xTaskCreate(webpage_task, "Webpage Task", 8192, NULL, 5, &webpage_task_handle);
ESP_LOGI(TAG, "Webpage task created");

}

void webpage_task(void* pvParameters){
    GPSData web_curr_pos_gps_data;
    ESP_LOGI(TAG, "Webpage task started");
    httpd_handle_t server = start_webserver();

    if (server == NULL) {
        ESP_LOGE(TAG, "Failed to start web server");
        return;
    } else {
        ESP_LOGI(TAG, "Web server started");
    }

    while(1){
        // Gets the systems data to output in the browser

        if(xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE){
            //ESP_LOGI(TAG, "Test getting mutex on the gps_data");
            web_data_to_send_snapshot.web_gps_data.altitude = gps_data.altitude;
            web_data_to_send_snapshot.web_gps_data.latitude = gps_data.latitude;
            web_data_to_send_snapshot.web_gps_data.longitude = gps_data.longitude;
            web_data_to_send_snapshot.web_gps_data.HDOP = gps_data.HDOP;
            web_data_to_send_snapshot.web_gps_data.satellite_number = gps_data.satellite_number;
            web_data_to_send_snapshot.web_gps_data.RTK_fix = gps_data.RTK_fix;
            xSemaphoreGive(gps_data_mutex);
        }

         if(xSemaphoreTake(plant_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE){
            
            web_data_to_send_snapshot.web_plant_data.current_acc_distance = plant_data.current_acc_distance;
            web_data_to_send_snapshot.web_plant_data.numPlants = plant_data.numPlants;
            web_data_to_send_snapshot.web_plant_data.plant_mode_active = plant_data.plant_mode_active;
            
            xSemaphoreGive(plant_data_mutex);
        }

        if (xSemaphoreTake(web_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            
            web_data_to_send.web_gps_data = web_data_to_send_snapshot.web_gps_data;
            web_data_to_send.web_plant_data = web_data_to_send_snapshot.web_plant_data;
     
            //ESP_LOGI(TAG, "Test getting mutex on the settings_data");
            xSemaphoreGive(web_data_mutex);
        } else {
            ESP_LOGW(TAG, "Failed to take settings_data_mutex");
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms to avoid busy waiting

    }
}

