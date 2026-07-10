
/*
    Author: António Malato
    This is a implementation file for the state_machine

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    26/6/2026 - Implementation of the state_machine_loop and set_led_planting_mode functions



*/

#include "common.h"

void set_led_planting_mode(bool on)
{

    gpio_set_level(GREEN_LED_PIN, on);
    gpio_set_level(RED_LED_PIN, !on);
}

void set_led_wait_for_planting_mode()
{
    gpio_set_level(YELLOW_LED_PIN, true);
}

void state_machine_loop(uint64_t* num_plantings,state_machine_data_t *next_states, sys_data_t sys_data_copy, gps_tracker_t *gps_tracker)
{
    // Implement the state machine logic here
    // For example, check the current state and perform actions based on the state
    // Transition to other states based on events or conditions
    uint16_t local_delta_calculated = 0;
    switch (next_states->next_state)
    {
    case PLANT_MODE:
        set_led_planting_mode(true);
        next_states->next_sub_state = IDLE_SUBSTATE;

        switch (next_states->next_sub_state)
        {
        case WAIT_FOR_PLANT:

            ESP_LOGI("STATE MACHINE LOOP", "WAITING FOR PLANT");
            set_led_wait_for_planting_mode();
            // wait for 2 seconds, consider the planting done
            vTaskDelay(pdMS_TO_TICKS(2000));
            next_states->next_sub_state = NORMAL_OPERATION;
            break;

        case NORMAL_OPERATION:
            // configurar o calcular da distancia acumulada guardar na variável geral de gps_tracker
            // se tiver no valor pretendido de distancia acumulada proximo sub_state é WAIT_FOR_PLANT
            local_delta_calculated = calculate_accumulated_distance(gps_tracker->last_position, gps_tracker->current_position);
            if (local_delta_calculated > sys_data_copy.allowed_delta_plant_error)
            {   
                gps_tracker->accumulated_distance = gps_tracker->accumulated_distance + local_delta_calculated;
                
                
                if (gps_tracker->accumulated_distance >= sys_data_copy.delta_pos)
                {
                    next_states->next_sub_state = WAIT_FOR_PLANT;
                    // 1 second 3.3V pulse to activate the planting mechanism
                    gpio_set_level(ACTIVATE_PLANTING, true);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    gpio_set_level(ACTIVATE_PLANTING, false);
                    (*num_plantings)++;
                    gps_tracker->accumulated_distance = gps_tracker->accumulated_distance - sys_data_copy.delta_pos;
                }
                else
                {
                    next_states->next_sub_state = NORMAL_OPERATION;
                }
            }

            break;

        default:

            next_states->next_sub_state = NORMAL_OPERATION;
            break;
        }

        break;

    case IDLE:
        set_led_planting_mode(false);
        next_states->next_sub_state = IDLE_SUBSTATE;
        break;

    default:
        ESP_LOGW("state machine code", "ERROR ENTERED DEFAULT STATE");
        break;
    }
}


