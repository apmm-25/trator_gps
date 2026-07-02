/* 
    Author: António Malato
    This is the main entry point c implementation file

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    24/6/2026 - Added the creation of the 3 tasks for button management, localization and webpage via wifi
    


*/
#include "common.h"
#include "button_task.h"
#include "localization_task.h"
#include "web_page_management_task.h"
#include "state_machine_task.h"


static const char *TAG = "APP_MAIN";

SemaphoreHandle_t gps_data_mutex;
GPSData gps_data;
SemaphoreHandle_t settings_data_mutex;
web_cmds_t received_settings_data;
web_data_t web_data_to_send;
EventGroupHandle_t system_events;
sys_data_t system_data;


void GPIO_init(void){

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



void default_system_data_init(void){
    
    system_data.delta_pos = DEFAULT_DELTA_POS;
    system_data.plant_time = DEFAULT_PLANT_TIME;
    gps_data.altitude = GPS_NULL_ISLAND_ALTITUDE;
    gps_data.latitude = GPS_NULL_ISLAND_LATITUDE;
    gps_data.longitude = GPS_NULL_ISLAND_LONGITUDE;
    
    
    return;
}

void mutexes_init(void){
    gps_data_mutex = xSemaphoreCreateMutex();

    if (gps_data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create gps_data_mutex");
        return;
    } else {
        ESP_LOGI(TAG, "gps_data_mutex created");
    }

    settings_data_mutex = xSemaphoreCreateMutex();
    if (settings_data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create settings_data_mutex");
        return;
    } else {
        ESP_LOGI(TAG, "settings_data_mutex created");
    }
}

void event_group_init(void){
    system_events = xEventGroupCreate();
    if (system_events == NULL) {
        ESP_LOGE(TAG, "Failed to create system_events");
        return;
    } else {
        ESP_LOGI(TAG, "system_events created");
    }

}



void app_main(void)
{
    // WIFI 

    // GPIO INITIALIZATION
    GPIO_init();
    
    // MUTEX INITIALIZATION
    mutexes_init();

    // EVENT GROUP INITIALIZATION
    event_group_init();

    // SET THE DEFAULT VALUES FOR THE SYSTEM DATA
    default_system_data_init();

    //TASK INITIALIZATION
    button_task_start();
    localization_task_start();
    webpage_task_start();
    state_machine_task_start();
    ESP_LOGI(TAG, "Tasks created");


}
