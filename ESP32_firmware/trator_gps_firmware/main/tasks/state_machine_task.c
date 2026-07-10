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

void gps_data_distance_test(GPSData Data1, GPSData Data2)
{
    double distance = calculate_accumulated_distance(Data1, Data2);
    ESP_LOGI(TAG, "Distance between points: %.2f meters", distance);
}

void state_machine_task_start(void)
{

    TaskHandle_t state_machine_task_handle;
    xTaskCreate(state_machine_task, "State Machine Task", 4096, NULL, 5, &state_machine_task_handle);
    if (state_machine_task_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to create state machine task");
        return;
    }
    else
    {

        ESP_LOGI(TAG, "State machine task created");
    }
}

void state_machine_task(void *pvParameters)
{
    ESP_LOGI(TAG, "State machine task started");
    GPSData curr_pos_gps_data;
    uint64_t num_plantings = 0;
    curr_pos_gps_data.altitude = 0;
    curr_pos_gps_data.latitude = 0;
    curr_pos_gps_data.longitude = 0;
    curr_pos_gps_data.HDOP = 0;
    curr_pos_gps_data.satellite_number = 0;
    gps_tracker_t gps_tracker;
    gps_tracker.first_sample = true;
    gps_tracker.accumulated_distance = 0;
    gps_tracker.last_position = curr_pos_gps_data;
    gps_tracker.current_position = curr_pos_gps_data;
    state_machine_data_t curr_state;
    curr_state.next_state = IDLE;
    curr_state.next_sub_state = IDLE_SUBSTATE;
    sys_data_t system_data_snapshot;

    while (1)
    {
        // Here the functions of the state machine will be used to always be managing the states of the tractor
        // For example, read the state machine data and update the state of the tractor in a global variable or a queue

        if (xSemaphoreTake(sys_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            system_data_snapshot.delta_pos = received_settings_data.delta_pos;
            system_data_snapshot.plant_time = received_settings_data.plant_time;
            system_data_snapshot.allowed_delta_plant_error = received_settings_data.allowed_delta_plant_error;

            // ESP_LOGI(TAG, "Test getting mutex on the settings_data");
            xSemaphoreGive(sys_data_mutex);
        }
        else
        {
            ESP_LOGW(TAG, "Failed to take sys_data_mutex");
        }
        

        // ESP_LOGI(TAG, "Received the settings: delta: %d, plant time %d, allowed error: %.2f", system_data_snapshot.delta_pos, );
        ESP_LOGI(TAG, " state machine task side received system_data: \n delta_pos: %d, \n plant_time: %d, \n allowed_error: %.f", system_data_snapshot.delta_pos, system_data_snapshot.plant_time, system_data_snapshot.allowed_delta_plant_error);
        if (xSemaphoreTake(plant_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            plant_data.current_acc_distance = gps_tracker.accumulated_distance;
            plant_data.numPlants = num_plantings;
            if (curr_state.next_state == PLANT_MODE)
            {
                plant_data.plant_mode_active = true;
            }
            else
            {
                plant_data.plant_mode_active = false;
            }
            xSemaphoreGive(plant_data_mutex);
        }
        else
        {
            ESP_LOGW(TAG, "Failed to take plant_data_mutex");
        }

        if (xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            // ESP_LOGI(TAG, "Test getting mutex on the gps_data");
            curr_pos_gps_data.altitude = gps_data.altitude;
            curr_pos_gps_data.latitude = gps_data.latitude;
            curr_pos_gps_data.longitude = gps_data.longitude;
            curr_pos_gps_data.HDOP = gps_data.HDOP;
            curr_pos_gps_data.satellite_number = gps_data.satellite_number;
            xSemaphoreGive(gps_data_mutex);
        }

        ESP_LOGI(TAG, "Received current GPS Position: Latitude: %.6f, Longitude: %.6f, Altitude: %.2f", curr_pos_gps_data.latitude, curr_pos_gps_data.longitude, curr_pos_gps_data.altitude);
        // read the system_state
        EventBits_t bits = xEventGroupGetBits(system_events);
        if (bits & EVENT_PLANT_MODE)
        {
            curr_state.next_state = PLANT_MODE;
        }
        else
        {
            curr_state.next_state = IDLE;
            curr_state.next_sub_state = IDLE_SUBSTATE;
        }
        // call next state
        gps_tracker.last_position = gps_tracker.current_position;
        gps_tracker.current_position = curr_pos_gps_data;
        state_machine_loop(&num_plantings, &curr_state, system_data_snapshot, &gps_tracker);

        // ESP_LOGI(TAG, "State machine task running");

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000 ms to avoid busy waiting
    }
}