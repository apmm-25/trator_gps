/*
    Author: António Malato
    This is the implementation file for functions of the web app

    If a change is made add the day and the title of the change here:


    05/7/2026 - Added the start webserver function


*/
#include "web_app.h"

static esp_err_t main_page_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

bool webpage_input_validity_check(web_cmds_t web_cmds)
{

    if (web_cmds.delta_pos == 0)
    {
        return false;
    }
    else if (web_cmds.plant_time == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static esp_err_t get_handler_outputs(httpd_req_t *req)
{
    // gets the output data the browser needs to get

    cJSON *root = cJSON_CreateObject();
    web_data_t web_data_to_send_local;

    if (xSemaphoreTake(web_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {

        web_data_to_send_local.web_gps_data = web_data_to_send.web_gps_data;
        web_data_to_send_local.web_plant_data = web_data_to_send.web_plant_data;

        // ESP_LOGI(TAG, "Test getting mutex on the settings_data");
        xSemaphoreGive(web_data_mutex);
    }
    else
    {
        ESP_LOGW("web_get_handler", "Failed to take settings_data_mutex");
    }

    cJSON_AddBoolToObject(root, "plant_mode_active", web_data_to_send_local.web_plant_data.plant_mode_active);
    cJSON_AddNumberToObject(root, "totalPlantings", web_data_to_send_local.web_plant_data.numPlants);
    cJSON_AddNumberToObject(root, "acc_distance", web_data_to_send_local.web_plant_data.current_acc_distance);

    cJSON *gps_data_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(gps_data_json, "lat", web_data_to_send_local.web_gps_data.latitude);
    cJSON_AddNumberToObject(gps_data_json, "lon", web_data_to_send_local.web_gps_data.longitude);
    cJSON_AddNumberToObject(gps_data_json, "alt", web_data_to_send_local.web_gps_data.altitude);
    cJSON_AddNumberToObject(gps_data_json, "hdop", web_data_to_send_local.web_gps_data.HDOP);
    cJSON_AddNumberToObject(gps_data_json, "sat", web_data_to_send_local.web_gps_data.satellite_number);

    // Attach GPS object to root
    cJSON_AddItemToObject(root, "gps_data", gps_data_json);

    httpd_resp_set_type(
        req,
        "application/json");

    // Transforms the JSON object into a string
    char *json_response = cJSON_PrintUnformatted(root);

    // SEND IT
    if (json_response != NULL)
    {
        httpd_resp_send(
            req,
            json_response,
            HTTPD_RESP_USE_STRLEN);
        free(json_response);
    }
    else
    {
        ESP_LOGE("web_get_handler", "Failed to create JSON response");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "JSON allocation failed");
    }

    cJSON_Delete(root);

    return ESP_OK;
}

static esp_err_t post_handler_inputs(httpd_req_t *req)
{
    /*
        TO DO: Retrieve data from json buffer parsed into the structure
    */

    int data_len = req->content_len + 1;

    char web_data_buffer[data_len];
    int ret = httpd_req_recv(req, web_data_buffer, req->content_len);
    if (ret <= 0)
    {
        return ESP_FAIL;
    }

    web_data_buffer[ret] = '\0';

    // parse with JSON
    cJSON *received_root = cJSON_Parse(web_data_buffer);

    if (received_root == NULL)
    {
        ESP_LOGE("input handler", "Error return empty data from the browser");
        return ESP_FAIL;
    }
    else
    {
        cJSON *delta_pos_item = cJSON_GetObjectItem(received_root, "deltaPosition");
        cJSON *plantTime_item = cJSON_GetObjectItem(received_root, "plantTime");
        cJSON *reset_item = cJSON_GetObjectItem(received_root, "reset");

        if (cJSON_IsNumber(delta_pos_item))
        {

            web_page_received_settings_data_snapshot.delta_pos = (uint8_t)delta_pos_item->valueint;
        }
        else
        {

            ESP_LOGW("web_app_handler", "Retuning unexpected value from the browser in the delta_pos field");
        }

        if (cJSON_IsNumber(plantTime_item))
        {

            web_page_received_settings_data_snapshot.plant_time = (uint8_t)plantTime_item->valueint;
        }
        else
        {

            ESP_LOGW("web_app_handler", "Retuning unexpected value from the browser in the plant time field");
        }

        if (cJSON_IsBool(reset_item))
        {
            ESP_LOGW("web_app_handler", "Here is the value of the reset item: %d",  web_page_received_settings_data_snapshot.reset);
            web_page_received_settings_data_snapshot.reset = cJSON_IsTrue(reset_item);
        }
        else
        {

            ESP_LOGW("web_app_handler", "Retuning unexpected value from the browser in the reset item field");
        }

        // Only passes the received data if it has been checked
        if (webpage_input_validity_check(web_page_received_settings_data_snapshot))
        {
            // Gets the data from the browser and puts it in the received_settings_data variable
            if (xSemaphoreTake(sys_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
            {

                if (received_settings_data.delta_pos != web_page_received_settings_data_snapshot.delta_pos)
                {
                    received_settings_data.delta_pos = web_page_received_settings_data_snapshot.delta_pos;
                }

                if (received_settings_data.plant_time != web_page_received_settings_data_snapshot.plant_time)
                {
                    received_settings_data.plant_time = web_page_received_settings_data_snapshot.plant_time;
                }

                if (received_settings_data.reset != web_page_received_settings_data_snapshot.reset)
                {
                    received_settings_data.reset = web_page_received_settings_data_snapshot.reset;
                }

                xSemaphoreGive(sys_data_mutex);
            }
            else
            {
                ESP_LOGW("web_app_handler", "Failed to take sys_data_mutex");
            }
        }
    }

    cJSON_Delete(received_root);
    return ESP_OK;
}

httpd_handle_t start_webserver()
{
    static const httpd_uri_t main_page = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = main_page_handler,
        .user_ctx = NULL};

    static const httpd_uri_t uri_input = {
        .uri = "/inputs",
        .method = HTTP_POST,
        .handler = post_handler_inputs,
        .user_ctx = NULL};

    static const httpd_uri_t uri_output = {
        .uri = "/outputs",
        .method = HTTP_GET,
        .handler = get_handler_outputs,
        .user_ctx = NULL};
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI("WEB_APP", "Web server started");
        // Register URI handlers here if needed
        httpd_register_uri_handler(server, &main_page);
        httpd_register_uri_handler(server, &uri_input);
        httpd_register_uri_handler(server, &uri_output);

        return server;
    }
    else
    {
        ESP_LOGE("WEB_APP", "Failed to start web server");
        return NULL;
    }
    return server;
}
