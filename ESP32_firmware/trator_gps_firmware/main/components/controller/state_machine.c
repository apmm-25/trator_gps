
/* 
    Author: António Malato
    This is a implementation file for the state_machine 

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    26/6/2026 - Implementation of the state_machine_loop and set_led_planting_mode functions



*/

#include "common.h"

void set_led_planting_mode(bool on){

    gpio_set_level(GREEN_LED_PIN, on);
    gpio_set_level(RED_LED_PIN, !on);
}

void set_led_wait_for_planting_mode(){
    gpio_set_level(YELLOW_LED_PIN, true);
}

plant_mode_substate_t state_machine_loop(state_machine_data_t next_states, sys_data_t sys_data, gps_tracker_t* gps_tracker){
    // Implement the state machine logic here
    // For example, check the current state and perform actions based on the state
    // Transition to other states based on events or conditions
    plant_mode_substate_t substate = next_states.next_sub_state;
    state_machine_state_t next_state = next_states.next_state;
    switch (next_state)
    {
    case PLANT_MODE:
        set_led_planting_mode(true);
        switch (substate)
        {
        case WAIT_FOR_PLANT:
            /* code */
            ESP_LOGI("STATE MACHINE LOOP", "WAITING FOR PLANT");
            set_led_wait_for_planting_mode();
            // wait for 2 seconds, consider the planting done
            vTaskDelay(pdMS_TO_TICKS(2000));
            return NORMAL_OPERATION;
        
        case NORMAL_OPERATION:
            // configurar o calcular da distancia acumulada guardar na variável geral de gps_tracker
            // se tiver no valor pretendido de distancia acumulada proximo sub_state é WAIT_FOR_PLANT
            return NORMAL_OPERATION;
            break;

        default:
            return NORMAL_OPERATION;
        }


        break;
    
    case IDLE:
        set_led_planting_mode(false);
        return IDLE_SUBSTATE;
        /* code */
    
    default:
        return IDLE_SUBSTATE;

    }


}


