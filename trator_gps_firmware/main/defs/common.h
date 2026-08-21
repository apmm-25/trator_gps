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
#include "gps.h"
#include "types.h"
#include "libs.h"
#include "serial_comms.h"
#include "web_app.h"
#include "state_machine.h"

// GENERAL CONSTANTS

#define EARTH_RADIUS 6371000.0 // in meters

#define MAX_BUFFER_SIZE 256

#define HIGH_SIDE_MOVEMENT_ERROR 8 // in meters


// WIFI ACCESS POINT CONSTANTS

#define WIFI_SSID CONFIG_ESP_WIFI_SSID
#define WIFI_PASSWORD CONFIG_ESP_WIFI_PASSWORD
#define WIFI_CHANNEL CONFIG_ESP_WIFI_CHANNEL
#define MAX_STA_CONN CONFIG_ESP_MAX_STA_CONN


// LED INITIALIZATION

#define GREEN_LED_PIN GPIO_NUM_18
#define RED_LED_PIN GPIO_NUM_21
#define YELLOW_LED_PIN GPIO_NUM_19
#define BUTTON_IN GPIO_NUM_35
#define ACTIVATE_PLANTING GPIO_NUM_25

#define GPS_UART UART_NUM_2
#define GPS_UART_TX GPIO_NUM_17
#define GPS_UART_RX GPIO_NUM_16

#define DEFAULT_DELTA_POS 2 // Default delta position
#define DEFAULT_PLANT_TIME 50 // Default plant time in miliseconds
#define DEFAULT_PLANT_ERROR 0.4 // 10 cm 

#define YELLOW_LED_BLINK_TIME 1000 // 1 second blink time for the yellow led

#define EVENT_PLANT_MODE (1 << 0)
#define EVENT_YELLOW_LED (1 << 1)
#define EVENT_NEW_SETTINGS_DATA (1 << 2)
#define EVENT_REBOOT (1 << 3)
#define BUTTON_PRESS_TIME 2000 // 2 seconds click


// GPS constants

#define GPS_BEGIN_RECEIVE '$'
#define GPS_LINE_TERMINATOR '\n'
#define GPS_DATA_FIELD_TERMINATOR ','
#define GPS_CHECKSUM_FIELD_INDICATOR '*'
#define GPS_DATA_INITIAL_CHAR '$'
#define PUBX_MSG_TYPE_ID "PUBX"
#define GNS_MSG_TYPE_ID "GNS"
#define GNS_MSG_RTK_FIXED_TYPE 'R'
#define GNS_MSG_RTK_FLOAT_TYPE 'F'
#define GNS_MSG_PRECISION_TYPE 'P'
#define GNS_MSG_DIFFERENTIAL_TYPE 'D'
#define GNS_MSG_AUTONOMOUS_TYPE 'A'
#define GGA_MSG_TYPE_ID "GGA"
#define GLL_MSG_TYPE_ID "GLL"
#define GPS_DATA_FIELD_NORTH_INDICATOR 'N'
#define GPS_DATA_FIELD_SOUTH_INDICATOR 'S'
#define GPS_DATA_FIELD_EAST_INDICATOR 'E'
#define GPS_DATA_FIELD_WEST_INDICATOR 'W'
#define GPS_DATA_FIELD_MAX_STR_LEN 25
#define GPS_NULL_ISLAND_LATITUDE 0.0
#define GPS_NULL_ISLAND_LONGITUDE 0.0
#define GPS_NULL_ISLAND_ALTITUDE 0.0
#define GPS_DEFAULT_HDOP 0.0
#define GPS_DEFAULT_SAT 0
#define HDOP_MAX_LIMIT 3


// Extern variables
extern SemaphoreHandle_t plant_data_mutex;
extern plant_data_t plant_data;
extern SemaphoreHandle_t gps_data_mutex;
extern GPSData gps_data;
extern SemaphoreHandle_t sys_data_mutex;
extern SemaphoreHandle_t web_data_mutex;
extern web_cmds_t received_settings_data;
extern web_data_t web_data_to_send;
extern EventGroupHandle_t system_events;
extern EventGroupHandle_t wifi_events;
extern sys_data_t system_data;
extern button_t button_state;





#endif /* COMMON_H */