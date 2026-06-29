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

void localization_task_start(void)
{

    TaskHandle_t localization_task_handle;
    xTaskCreate(localization_task, "Localization Task", 4096, NULL, 5, &localization_task_handle);
    ESP_LOGI(TAG, "Localization task created");
}

void localization_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Localization task started");
    GPSData curr_position;
    uint8_t rx_buffer[256];
    uint8_t next_byte;
    uint8_t last_byte;
    uint8_t index;
    zedf9p_incoming_data_t incoming_data;
    gps_msg_t gps_msg_type;
    nmea_raw_data_struct raw_data;
    raw_data.index = 0;
    raw_data.line_len = 0;

    while (1)
    {
        // Here the functions of the gps and other sensors will be used to always be calculating the position of the tractor
        // For example, read the gps data and update the position of the tractor in a global variable or a queue
        ESP_LOGI(TAG, "Localization task running");

        if (zedf9p_data_receiver(&raw_data))
        {
            // data corruption check
            if (nmea_checksum_comparison(&raw_data))
            {
                // Check ID

                gps_msg_type = check_gps_type(raw_data.rx_buffer);
                // passar o parser
                switch (gps_msg_type)
                {
                case GNS:
                    incoming_data = GNS_parser(&raw_data);
                    break;
                case PUBX:
                    //incoming_data = PUBX_parser(&raw_data);
                    break;

                case UNDEFINED:
                    break;

                default:
                    break;
                }
                
                // reset index on the string
                index = 0;

                if (xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
                {
                    ESP_LOGI(TAG, "Test getting mutex on the gps_data");
                    gps_data.altitude = incoming_data.altitude;
                    gps_data.latitude = incoming_data.latitude;
                    gps_data.longitude = incoming_data.longitude;
                }
                else
                {
                    ESP_LOGW(TAG, "Failed to take gps data mutex");
                }

            } else {

                ESP_LOGW(TAG, "Skipping corrupted NMEA data");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1)); // Delay for 1 ms to avoid busy waiting
    }
}