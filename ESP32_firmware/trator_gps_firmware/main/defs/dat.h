
/* 
    Author: António Malato
    This is a header file that defines project data types used across the project
    Structures are defined here
    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    


*/

#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>

typedef struct {
   serial_comms_type_t comms_type; 
   uint32_t baudrate;
   uint8_t parity;
   uint8_t stop_bits;
   serial_comm_set_status_t set_status;
} serial_comms_config_t;



typedef struct {
    bool first_sample;
    uint16_t accumulated_distance;
} gps_tracker_t;