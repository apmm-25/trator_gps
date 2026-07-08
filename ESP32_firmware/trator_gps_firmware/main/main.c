/*
    Author: António Malato
    This is the main entry point c implementation file

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    24/6/2026 - Added the creation of the 3 tasks for button management, localization and webpage via wifi
    05/7/2026 - Added the WiFi Access Point initialization function

*/
#include "common.h"
#include "button_task.h"
#include "localization_task.h"
#include "web_page_management_task.h"
#include "state_machine_task.h"

static const char *TAG = "APP_MAIN";

SemaphoreHandle_t gps_data_mutex;
GPSData gps_data;
SemaphoreHandle_t sys_data_mutex;
web_cmds_t received_settings_data;
web_data_t web_data_to_send;
EventGroupHandle_t system_events;
sys_data_t system_data;
SemaphoreHandle_t web_data_mutex;
plant_data_t plant_data;
SemaphoreHandle_t plant_data_mutex;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI("WIFI AP", " New Station connected  AID=%d", event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI("WIFI AP", " Station disconnected  AID=%d", event->aid);
    }
}

void wifi_init_ap()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    esp_netif_ip_info_t ip;

    ESP_ERROR_CHECK(esp_netif_get_ip_info(ap_netif, &ip));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASSWORD,
            .channel = WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };

    if (strlen(WIFI_PASSWORD) == 0)
    {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WIFI AP", "WiFi initialized in AP mode with SSID: %s, Password: %s", WIFI_SSID, WIFI_PASSWORD);
    ESP_LOGI("WIFI AP", "AP IP address: " IPSTR, IP2STR(&ip.ip));
}

void GPIO_init(void)
{

    gpio_config_t io_config = {};
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.intr_type = GPIO_INTR_DISABLE;
    io_config.pull_up_en = 0;
    io_config.pull_down_en = 0;

    // green led config
    io_config.pin_bit_mask = (1ULL << GREEN_LED_PIN);
    gpio_config(&io_config);

    // red led config
    io_config.pin_bit_mask = (1ULL << RED_LED_PIN);
    gpio_config(&io_config);

    // yellow led config
    io_config.pin_bit_mask = (1ULL << YELLOW_LED_PIN);
    gpio_config(&io_config);

    // button input setup
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pin_bit_mask = (1ULL << BUTTON_IN);
    gpio_config(&io_config);
}

void default_system_data_init(void)
{
    // Initializing the shared variables to their defaults on startup

    system_data.delta_pos = DEFAULT_DELTA_POS;
    system_data.plant_time = DEFAULT_PLANT_TIME;
    system_data.allowed_delta_plant_error = DEFAULT_PLANT_ERROR;

    gps_data.altitude = GPS_NULL_ISLAND_ALTITUDE;
    gps_data.latitude = GPS_NULL_ISLAND_LATITUDE;
    gps_data.longitude = GPS_NULL_ISLAND_LONGITUDE;
    gps_data.HDOP = GPS_DEFAULT_HDOP;
    gps_data.satellite_number = GPS_DEFAULT_SAT;

    plant_data.current_acc_distance = 0;
    plant_data.numPlants = 0;
    plant_data.plant_mode_active = false;

    return;

}

void mutexes_init(void)
{
    gps_data_mutex = xSemaphoreCreateMutex();

    if (gps_data_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create gps_data_mutex");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "gps_data_mutex created");
    }

    sys_data_mutex = xSemaphoreCreateMutex();
    if (sys_data_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create settings_data_mutex");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "settings_data_mutex created");
    }

    web_data_mutex = xSemaphoreCreateMutex();
    if (web_data_mutex == NULL) {

        ESP_LOGE(TAG, "Failed to create web_data_mutex");
        return;
    } 
    else {
        ESP_LOGI(TAG, "web_data_mutex created");
    }

    plant_data_mutex = xSemaphoreCreateMutex();
    if (plant_data_mutex == NULL) {

        ESP_LOGE(TAG, "Failed to create plant_data_mutex");
        return;
    } 
    else {
        ESP_LOGI(TAG, "plant_data_mutex created");
    }

}

void event_group_init(void)
{
    system_events = xEventGroupCreate();
    if (system_events == NULL)
    {
        ESP_LOGE(TAG, "Failed to create system_events");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "system_events created");
    }
}

void app_main(void)
{

    // GPIO INITIALIZATION
    GPIO_init();

    // MUTEX INITIALIZATION
    mutexes_init();

    // EVENT GROUP INITIALIZATION
    event_group_init();

    // SET THE DEFAULT VALUES FOR THE SYSTEM DATA
    default_system_data_init();

    // WIFI
    wifi_init_ap();

    // TASK INITIALIZATION
    button_task_start();
    localization_task_start();
    webpage_task_start();
    state_machine_task_start();
    ESP_LOGI(TAG, "Tasks created");
}
