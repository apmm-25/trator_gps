/* 
    Author: António Malato
    This is a header file for the state_machine implementation file

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file

*/

#ifndef  STATE_MACHINE_H
#define  STATE_MACHINE_H

#include "common.h"

void state_machine_loop();
void set_led_planting_mode(bool on);
#endif /* STATE_MACHINE_H */