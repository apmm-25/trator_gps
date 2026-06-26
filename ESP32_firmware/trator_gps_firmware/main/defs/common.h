/* 
    Author: António Malato
    This is a header file for common data types used across the project
    Events, semaphores, mutexes, queues and other common data types are defined here

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    24/6/2026 - Sub divided the common data types into separated files


*/



#ifndef COMMON_H
#define COMMON_H


#include "dat.h"
#include "types.h"
#include "libs.h"
#include "serial_comms.h"


// LED INITIALIZATION

#define GREEN_LED_PIN GPIO_NUM_18
#define RED_LED_PIN GPIO_NUM_21
#define YELLOW_LED_PIN GPIO_NUM_19
#define BUTTON_IN GPIO_NUM_35

#define GPS_UART UART_NUM_2
#define GPS_UART_TX GPIO_NUM_17
#define GPS_UART_RX GPIO_NUM_16

#define DEFAULT_DELTA_POS 2 // Default delta position
#define DEFAULT_PLANT_TIME 2000 // Default plant time in miliseconds

#define EVENT_PLANT_MODE (1 << 0)
#define EVENT_NEW_GPS_DATA (1 << 1)
#define EVENT_NEW_SETTINGS_DATA (1 << 2)
#define BUTTON_PRESS_TIME 2000 // 2 miliseconds click

extern SemaphoreHandle_t gps_data_mutex;
extern GPSData gps_data;
extern SemaphoreHandle_t settings_data_mutex;
extern web_cmds_t received_settings_data;
extern web_data_t web_data_to_send;
extern EventGroupHandle_t system_events;
extern sys_data_t system_data;
extern button_t button_state;




#endif /* COMMON_H */