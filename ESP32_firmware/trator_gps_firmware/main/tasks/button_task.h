/* 
    Author: António Malato
    This is a header file for the button_task implementation file

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file



*/

#ifndef BUTTON_LOGIC_TASK_H
#define BUTTON_LOGIC_TASK_H

#include "state_machine.h"

void button_task_start(void);
void button_task(void* pvParameters);

#endif /* BUTTON_LOGIC_TASK_H */