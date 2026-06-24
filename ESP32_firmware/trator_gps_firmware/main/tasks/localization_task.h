/* 
    Author: António Malato
    This is a header file for the localization_task implementation file

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file



*/

#ifndef LOCALIZATION_TASK_H
#define LOCALIZATION_TASK_H

#include "gps.h"

void localization_task_start(void);
void localization_task(void* pvParameters);

#endif /* LOCALIZATION_TASK_H */