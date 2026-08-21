
/*
    Author: António Malato
    This is a implementation file for the state_machine

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    26/6/2026 - Implementation of the state_machine_loop and set_led_planting_mode functions
    12/7/2026 - Fixed bugs in GPS delta distance calculation


*/

#include "common.h"

void set_led_planting_mode(bool on)
{

    gpio_set_level(GREEN_LED_PIN, on);
    gpio_set_level(RED_LED_PIN, !on);
}

void set_led_wait_for_planting_mode(bool on)
{
    gpio_set_level(YELLOW_LED_PIN, on);
}

void trigger_planting(state_machine_data_t *next_states, uint64_t *num_plantings, uint64_t *num_plantings_crop_row, gps_tracker_t *gps_tracker, sys_data_t sys_data_copy)
{
    next_states->next_sub_state = WAIT_FOR_PLANT;
    // 1 second 3.3V pulse to activate the planting mechanism
    // acrescentar o mutex. 
    xEventGroupSetBits(system_events, EVENT_YELLOW_LED); 
    gpio_set_level(ACTIVATE_PLANTING, true);
    vTaskDelay(pdMS_TO_TICKS(sys_data_copy.plant_time));
    gpio_set_level(ACTIVATE_PLANTING, false);
    (*num_plantings)++;
    (*num_plantings_crop_row)++;
    gps_tracker->accumulated_distance = gps_tracker->accumulated_distance - sys_data_copy.delta_pos;

}

// might be needed in the future if a way to confirm the planting is created
void wait_for_plant_subcase(state_machine_data_t *next_states)
{
    ESP_LOGI("STATE MACHINE LOOP", "WAITING FOR PLANT");
    next_states->next_sub_state = NORMAL_OPERATION;
}

bool first_sample_check(gps_tracker_t *gps_tracker)
{
    if (fabs(gps_tracker->anchor_position.latitude) < 1e-9 &&
        fabs(gps_tracker->anchor_position.longitude) < 1e-9 &&
        fabs(gps_tracker->anchor_position.altitude) < 1e-9)
    {
        return true;
    }
    else if (fabs(gps_tracker->current_position.latitude) < 1e-9 &&
             fabs(gps_tracker->current_position.longitude) < 1e-9 &&
             fabs(gps_tracker->current_position.altitude) < 1e-9)
    {
        return true;
    }
    else if (gps_tracker->current_position.HDOP < HDOP_MAX_LIMIT)
    {

        gps_tracker->anchor_position.altitude = gps_tracker->current_position.altitude;
        gps_tracker->anchor_position.longitude = gps_tracker->current_position.longitude;
        gps_tracker->anchor_position.latitude = gps_tracker->current_position.latitude;
        gps_tracker->anchor_position.HDOP = gps_tracker->current_position.HDOP;
        return true;
    }
    else
    {
        return false;
    }
}

void normal_operation_subcase(state_machine_data_t *next_states, gps_tracker_t *gps_tracker, sys_data_t sys_data_copy, uint64_t *num_plantings, uint64_t *num_plantings_crop_row)
{
   
    double local_delta_calculated = 0;
    // Check if it is the first sample, sets the variable
    gps_tracker->first_sample = first_sample_check(gps_tracker);

    local_delta_calculated = calculate_accumulated_distance(gps_tracker->anchor_position, gps_tracker->current_position, gps_tracker->first_sample);
    ESP_LOGI("State Machine Loop", "Current delta calculated: %.9f", local_delta_calculated);

    // ver como melhorar isto
    if (local_delta_calculated > DEFAULT_PLANT_ERROR && local_delta_calculated < HIGH_SIDE_MOVEMENT_ERROR)
    {
        gps_tracker->accumulated_distance = gps_tracker->accumulated_distance + local_delta_calculated;
        gps_tracker->anchor_position.latitude = gps_tracker->current_position.latitude;
        gps_tracker->anchor_position.longitude = gps_tracker->current_position.longitude;
        gps_tracker->anchor_position.altitude = gps_tracker->current_position.altitude;

        ESP_LOGI("State Machine Loop", "Accumulated distance is %f // Delta is %f //", gps_tracker->accumulated_distance, sys_data_copy.delta_pos);

        // gps_tracker->accumulated_distance

        if (gps_tracker->accumulated_distance >= (double)sys_data_copy.delta_pos)
        {
            ESP_LOGI("State Machine Loop", "Dei trigger ao plant, acc distance was %f", gps_tracker->accumulated_distance);
            trigger_planting(next_states, num_plantings, num_plantings_crop_row, gps_tracker, sys_data_copy);
        }
        else
        {
            next_states->next_sub_state = NORMAL_OPERATION;
        }
    }
    else if (local_delta_calculated >= HIGH_SIDE_MOVEMENT_ERROR)
    {
        // Ver o que adicionar aqui
        ESP_LOGW("State Machine Loop", "Probably HIGH SIDE Noise, value detected: %f, recheck the error recheck the high side error tolerance ", local_delta_calculated);
    }
    else
    {

        ESP_LOGW("State Machine Loop", "Probably Noise, value detected: %f", local_delta_calculated);
    }
}
void state_machine_loop(uint64_t *num_plantings, uint64_t *num_plantings_crop_row, state_machine_data_t *next_states, sys_data_t sys_data_copy, gps_tracker_t *gps_tracker)
{
    // Implement the state machine logic here
    // For example, check the current state and perform actions based on the state
    // Transition to other states based on events or conditions

    // ESP_LOGI("State Machine Loop", "state inside the switch: %d", next_states->next_state);
    switch (next_states->next_state)
    {
    case PLANT_MODE:
        set_led_planting_mode(true);

        switch (next_states->next_sub_state)
        {
        case WAIT_FOR_PLANT:
            ESP_LOGI("State Machine Loop", "Wait for Plant");
            wait_for_plant_subcase(next_states);
            break;

        case NORMAL_OPERATION:
            // configurar o calcular da distancia acumulada guardar na variável geral de gps_tracker
            // ESP_LOGI("State Machine Loop", "Normal Operation");
            normal_operation_subcase(next_states, gps_tracker, sys_data_copy, num_plantings, num_plantings_crop_row);
            break;

        default:
            next_states->next_sub_state = NORMAL_OPERATION;
            break;
        }

        break;

    case IDLE:
        set_led_planting_mode(false);
        next_states->next_sub_state = IDLE_SUBSTATE;
        gps_tracker->accumulated_distance = 0.0;
        gps_tracker->anchor_position = gps_tracker->current_position; // Reset anchor position to current position when entering IDLE
        *num_plantings_crop_row = 0;                                  // Reset the number of plantings in the current crop row when entering IDLE
        ESP_LOGI("State Machine Loop", "IDLE STATE");
        break;

    default:
        ESP_LOGW("state machine code", "ERROR ENTERED DEFAULT STATE");
        break;
    }
}
