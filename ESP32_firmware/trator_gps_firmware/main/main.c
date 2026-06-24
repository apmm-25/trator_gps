/* 
    Author: António Malato
    This is the main entry point c implementation file

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    24/6/2026 - Added the creation of the 3 tasks for button management, localization and webpage via wifi
    


*/
#include "common.h"


static const char *TAG = "APP_MAIN";

void app_main(void)
{
    // Inicializar gpio, WIFI, 

    
    button_task_start();
    localization_task_start();
    webpage_task_start();
    ESP_LOGI(TAG, "Tasks created");


}
