/* 
    Author: António Malato
    This is the main entry point c implementation file

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    24/6/2026 - Added the creation of the 3 tasks for button management, localization and webpage via wifi
    


*/

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"


void app_main(void)
{
    // Inicializar gpio, WIFI, 

    // Inicializar as 3 threads: button management, localization and webpage via wifi
    TaskHandle_t button_task_handle;
    TaskHandle_t localization_task_handle;
    TaskHandle_t webpage_task_handle;

    xTaskCreate(button_task, "Button Task", 2048, NULL, 5, &button_task_handle);
    xTaskCreate(localization_task, "Localization Task", 4096, NULL, 5, &localization_task_handle);
    xTaskCreate(webpage_task, "Webpage Task", 8192, NULL, 5, &webpage_task_handle);


}
