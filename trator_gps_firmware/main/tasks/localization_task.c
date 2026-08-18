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

void debug_function_treated(GPSData *treated_data)
{
    ESP_LOGI("GPS", "----------------------------");
    // ESP_LOGI("GPS", "Time       : %.2f", treated_data->timestamp);
    ESP_LOGI("GPS", "Latitude   : %.8f", treated_data->latitude);
    ESP_LOGI("GPS", "Longitude  : %.8f", treated_data->longitude);
    ESP_LOGI("GPS", "Satellites : %u", treated_data->satellite_number);
    ESP_LOGI("GPS", "Altitude   : %.8f", treated_data->altitude);
    ESP_LOGI("GPS", "HDOP   : %.2f", treated_data->HDOP);
    ESP_LOGI("GPS", "----------------------------");
}

void debug_function_received(zedf9p_incoming_data_t *data)
{
    ESP_LOGI("GPS", "----------------------------");
    ESP_LOGI("GPS", "Time       : %.2f", data->timestamp);
    ESP_LOGI("GPS", "Latitude   : %.8f %c", data->latitude, data->NS);
    ESP_LOGI("GPS", "Longitude  : %.8f %c", data->longitude, data->EW);
    ESP_LOGI("GPS", "Satellites : %u", data->satellite_number);
    ESP_LOGI("GPS", "Altitude   : %.2f", data->altitude);
    ESP_LOGI("GPS", "----------------------------");
}

void nmea_parser(nmea_raw_data_struct *raw_data)
{
    gps_msg_t gps_msg_type;
    zedf9p_incoming_data_t incoming_data;
    GPSData gps_data_local_task = {0};
    gps_data_local_task.RTK_fix = UNDEFINED_FIX;

    //ESP_LOGI("NMEA_PARSER", "Doing checksum comparison for string: %s", raw_data->rx_buffer);
    if (nmea_checksum_comparison(raw_data))
    {

        gps_msg_type = check_gps_type(raw_data->rx_buffer);

        raw_data->parser_index = 0; // Reset parser index before parsing
        //ESP_LOGW("GPS", "The GPS message type is: %d", gps_msg_type);
        if (gps_msg_type != UNDEFINED)
        {
            incoming_data = nmea_gps_type_loop(raw_data, gps_msg_type);
            
            //debug_function_received(&incoming_data);
            if (transform_into_decimal_degrees(incoming_data, &gps_data_local_task))
            {
                //debug_function_treated(&gps_data_local_task);
                if (xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
                {
                    gps_data.latitude = gps_data_local_task.latitude;
                    gps_data.altitude = gps_data_local_task.altitude;
                    gps_data.longitude = gps_data_local_task.longitude;
                    gps_data.HDOP = gps_data_local_task.HDOP;
                    gps_data.satellite_number = gps_data_local_task.satellite_number;
                    gps_data.RTK_fix = gps_data_local_task.RTK_fix;
                    xSemaphoreGive(gps_data_mutex);
                }
            }
            else
            {
                ESP_LOGI(TAG, "Undefined msg reference, skipping..");
            }
        }
    }
    else
    {

        ESP_LOGW("GPS", "THE CHECKSUM VALIDATION HAS FAILED");
    }
}

void localization_task_start(void)
{

    TaskHandle_t localization_task_handle;
    xTaskCreate(localization_task, "Localization Task", 4096, NULL, 5, &localization_task_handle);
    ESP_LOGI(TAG, "Localization task created");
}

nmea_raw_data_struct copy_data_for_processing(nmea_raw_data_struct *data)
{
    nmea_raw_data_struct ret;
    ret.index = data->index;
    ret.parser_index = data->parser_index;
    ret.line_len = data->line_len;
    memcpy(ret.rx_buffer, data->rx_buffer, sizeof(data->rx_buffer));
    return ret;
}


void localization_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Localization task started");
    nmea_raw_data_struct raw_data;      // to pass it for treatment and parsing
    nmea_raw_data_struct received_data; // to receive the data from the gps module
    raw_data.index = 0;
    raw_data.line_len = 0;
    received_data.index = 0;
    received_data.line_len = 0;
    bool receiving = false;

    while (1)
    {
        // Logic loop for the localization task
        if (zedf9p_data_receiver(&received_data, &receiving))
        {   

            raw_data = copy_data_for_processing(&received_data);
            //ESP_LOGI(TAG, "Here is the received data: %s", raw_data.rx_buffer );
            nmea_parser(&raw_data);
        }
        else
        {
            // ESP_LOGW(TAG, "Skipping corrupted NMEA data");
        }

        //vTaskDelay(pdMS_TO_TICKS(50));

        // Normal Delay for 1 ms to avoid busy waiting, 2 seconds to debug easier
    }
}
