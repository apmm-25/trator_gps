/* 
    Author: António Malato
    This is a header file for the state_machine_task implementation file

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file



*/

#ifndef STATE_MACHINE_TASK_H
#define STATE_MACHINE_TASK_H

#include "state_machine.h"
#include "common.h"

void state_machine_task_start();

void state_machine_task(void* pvParameters);

#endif /* STATE_MACHINE_TASK_H */