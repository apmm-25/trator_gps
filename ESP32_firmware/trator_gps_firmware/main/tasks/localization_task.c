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

void debug_function(zedf9p_incoming_data_t *treated_data)
{
    ESP_LOGI("GPS", "----------------------------");
    ESP_LOGI("GPS", "Time       : %.2f", treated_data->timestamp);
    ESP_LOGI("GPS", "Latitude   : %.4f %c", treated_data->latitude, treated_data->NS);
    ESP_LOGI("GPS", "Longitude  : %.4f %c", treated_data->longitude, treated_data->EW);
    ESP_LOGI("GPS", "Satellites : %u", treated_data->satellite_number);
    ESP_LOGI("GPS", "Altitude   : %.2f", treated_data->altitude);
    ESP_LOGI("GPS", "----------------------------");
}

void nmea_parser(nmea_raw_data_struct *raw_data)
{
    gps_msg_t gps_msg_type;
    zedf9p_incoming_data_t incoming_data;

    if (nmea_checksum_comparison(raw_data))
    {

        gps_msg_type = check_gps_type(raw_data->rx_buffer);
        raw_data->parser_index = 0; // Reset parser index before parsing
        switch (gps_msg_type)
        {
        case GNS:
            ESP_LOGI("GPS", "I Have entered the GNS_parser case");
            incoming_data = GNS_parser(raw_data);
            break;
        case PUBX:
            // incoming_data = PUBX_parser(raw_data);
            break;

        case UNDEFINED:
            break;

        default:
            break;
        }
        debug_function(&incoming_data);
    }
    else
    {

        // ESP_LOGW("GPS", "THE CHECKSUM VALIDATION HAS FAILED");
    }
}

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

    // invalid checksum
    char raw_buffer_test1[] = "$GNGNS,122310.20,3843.3382,N,00908.3581,W,AN,07,0.9,78.5,47.2,,*5F";
    // valid checksum
    char raw_buffer_test2[] = "$GNGNS,122310.20,3843.3382,N,00908.3581,W,AN,07,0.9,78.5,47.2,,*79";
    char raw_buffer_test3[] = "$GNGNS,1.1,2.2,N,3.3,E,A,4,5.5,6.6,7.7,8,9,A*00";

    while (1)
    {
        // Here the functions of the gps and other sensors will be used to always be calculating the position of the tractor
        // For example, read the gps data and update the position of the tractor in a global variable or a queue
        // ESP_LOGI(TAG, "Localization task running");
        strcpy(raw_data.rx_buffer, raw_buffer_test2);
        nmea_parser(&raw_data);
        /*

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
        */
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        // Normal Delay for 1 ms to avoid busy waiting, 2 seconds to debug easier
    }
}
