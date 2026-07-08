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

void gps_function_converted_data_debug(GPSData *gps_data)
{
    ESP_LOGI("GPS", "----------------------------");
    ESP_LOGI("GPS", "Latitude   : %.4f", gps_data->latitude);
    ESP_LOGI("GPS", "Longitude  : %.4f", gps_data->longitude);
    ESP_LOGI("GPS", "Altitude   : %.2f", gps_data->altitude);
    ESP_LOGI("GPS", "----------------------------");
}

void nmea_parser(nmea_raw_data_struct *raw_data)
{
    gps_msg_t gps_msg_type;
    zedf9p_incoming_data_t incoming_data;
    GPSData gps_data_local_task = {0};

    if (nmea_checksum_comparison(raw_data))
    {

        gps_msg_type = check_gps_type(raw_data->rx_buffer);
        raw_data->parser_index = 0; // Reset parser index before parsing
        //ESP_LOGW("GPS", "The GPS message type is: %d", gps_msg_type);

        incoming_data = nmea_gps_type_loop(raw_data, gps_msg_type);
        //debug_function(&incoming_data);
        gps_data_local_task = transform_into_decimal_degrees(incoming_data);
        //gps_function_converted_data_debug(&gps_data);
        if (xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            //ESP_LOGI(TAG, "Test getting mutex on the gps_data");
            gps_data.latitude = gps_data_local_task.latitude;
            gps_data.altitude = gps_data_local_task.altitude;
            gps_data.longitude = gps_data_local_task.longitude;
            gps_data.HDOP = gps_data_local_task.HDOP;
            gps_data.satellite_number = gps_data_local_task.satellite_number;
            xSemaphoreGive(gps_data_mutex);
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
    

    //// invalid checksum
    //char raw_buffer_test1[] = "$GNGNS,122310.20,3843.3382,N,00908.3581,W,AN,07,0.9,78.5,47.2,,*5F";
    //// valid checksum
    char raw_buffer_test2[] = "$GNGNS,122310.20,3843.3382,N,00908.3581,W,AN,07,0.9,78.5,47.2,,*79";
    //char raw_buffer_test3[] = "$GPGNS,122310.2,,,,,,07,,,,5.2,23,V*07";
    //char pubx_buffer_test[] = "$PUBX,00,081350.00,4717.113210,N,00833.915187,E,546.589,G3,2.1,2.0,0.007,77.52,0.007,,0.92,1.19,0.77,9,0,0*5F";
    memcpy(raw_data.rx_buffer, raw_buffer_test2, sizeof(raw_buffer_test2));


    while (1)
    {
        // Logic loop for the localization task
        //if (zedf9p_data_receiver(&received_data))
        //{
        //    raw_data = copy_data_for_processing(&received_data);
        //    nmea_parser(&raw_data);
        //}
        //else
        //{
        //    ESP_LOGW(TAG, "Skipping corrupted NMEA data");
        //}
        nmea_parser(&raw_data);
        
        vTaskDelay(pdMS_TO_TICKS(50));

        // Normal Delay for 1 ms to avoid busy waiting, 2 seconds to debug easier
    }

}
