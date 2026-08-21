/* 
    Author: António Malato
    This is a header file for to access exterior libraries across the project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    24/6/2026 - Sub divided the common data types into separated files


*/


#ifndef LIBS_H
#define LIBS_H

#include <stdint.h>
#include "types.h"
#include <math.h>

#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "cJSON.h"





#endif /* LIBS_H */