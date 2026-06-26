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
    if(state_machine_task_handle == NULL){
        ESP_LOGE(TAG, "Failed to create state machine task");
        return;
    } else {

        ESP_LOGI(TAG, "State machine task created");
    }
}

void state_machine_task(void* pvParameters){
    ESP_LOGI(TAG, "State machine task started");
    GPSData curr_pos_gps_data;
    GPSData last_position_gps_data;
    curr_pos_gps_data.altitude = 0;
    curr_pos_gps_data.latitude = 0;
    curr_pos_gps_data.altitude = 0;
    gps_tracker_t gps_tracker;
    state_machine_data_t curr_state;
    curr_state.next_state = IDLE;
    curr_state.next_sub_state = IDLE_SUBSTATE;
    state_machine_data_t previous_state;

    while(1){
        // Here the functions of the state machine will be used to always be managing the states of the tractor
        // For example, read the state machine data and update the state of the tractor in a global variable or a queue
        
        ESP_LOGI(TAG, "State machine task running");
        if (xSemaphoreTake(settings_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            system_data.delta_pos = received_settings_data.delta_pos;
            ESP_LOGI(TAG, "Test getting mutex on the settings_data");
            xSemaphoreGive(settings_data_mutex);
        } else {
            ESP_LOGW(TAG, "Failed to take settings_data_mutex");
        }

        if(xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE){
            ESP_LOGI(TAG, "Test getting mutex on the gps_data");
            //curr_pos_gps_data.altitude = gps_data.altitude;
            //curr_pos_gps_data.latitude = gps_data.latitude;
            //curr_pos_gps_data.longitude = gps_data.longitude;
        }

        // read the system_state
        EventBits_t bits = xEventGroupGetBits(system_events);
        if (bits && EVENT_PLANT_MODE){
            curr_state.next_state = PLANT_MODE;
        } else {
            curr_state.next_state = IDLE;
            curr_state.next_sub_state = IDLE_SUBSTATE;
        }
        // call next state
        state_machine_loop(&curr_state, system_data, &gps_tracker);
        vTaskDelay(pdMS_TO_TICKS(50)); // Delay for 50 ms to avoid busy waiting
    }
}