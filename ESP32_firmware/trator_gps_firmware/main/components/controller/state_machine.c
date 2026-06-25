
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

state_machine_state_t state_machine_loop(state_machine_state_t next_state, sys_data_t sys_data){
    // Implement the state machine logic here
    // For example, check the current state and perform actions based on the state
    // Transition to other states based on events or conditions
    plant_mode_substate_t substate = NORMAL_OPERATION;

    switch (next_state)
    {
    case PLANT_MODE:
        /* code */
        break;
    
    case IDLE:
        /* code */
    
    default:
        break;

    }

    return next_state;
    
}


