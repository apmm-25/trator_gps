/* 
    Author: António Malato
    This is a header file for the web page management function of the tractor project
    It has the declaration of the web page management thread.
    This webpage will be used to communicate with the tractor changes in its settings, and show some status information

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    
*/

#ifndef WEBPAGE_TASK_H
#define WEBPAGE_TASK_H

#include "web_app.h"
#include "common.h"

void webpage_task_start(void);
void webpage_task(void* pvParameters);

#endif // WEBPAGE_TASK_H