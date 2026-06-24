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


SemaphoreHandle_t gps_data_mutex;
SemaphoreHandle_t settings_data_mutex;
EventGroupHandle_t system_events;

#define EVENT_PLANT_MODE (1 << 0)
#define EVENT_NEW_GPS_DATA (1 << 1)
#define EVENT_NEW_SETTINGS_DATA (1 << 2)


#endif /* COMMON_H */