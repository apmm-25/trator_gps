
/* 
    Author: António Malato
    This is a implementation file for the state_machine 

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file



*/

#include "common.h"

void set_led_planting_mode(bool on){

    gpio_set_level(GREEN_LED_PIN, on);
    gpio_set_level(RED_LED_PIN, !on);
}

void state_machine_loop(){
    // Implement the state machine logic here
    // For example, check the current state and perform actions based on the state
    // Transition to other states based on events or conditions

    
}


