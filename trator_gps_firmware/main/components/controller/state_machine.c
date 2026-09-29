
/*
    Author: António Malato
    This is a implementation file for the state_machine

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    26/6/2026 - Implementation of the state_machine_loop and set_led_planting_mode functions
    12/7/2026 - Fixed bugs in GPS delta distance calculation
    29/9/2026 - Changes to the setup of the anchor point and first sample registration

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

GPSData position = gps_tracker->current_position;
    bool position_valid =
        position.HDOP < HDOP_MAX_LIMIT &&
        !(fabs(position.latitude) < 1e-9 &&
          fabs(position.longitude) < 1e-9 &&
          fabs(position.altitude) < 1e-9);

    if (!position_valid)
    {
        return true;
    }

    if (gps_tracker->first_sample)
    {
        gps_tracker->anchor_position = position;
        gps_tracker->first_sample = false;
        return true;
    }

    return false;


}

/* Checks if the RTK fix is accurate enough to consider the higher accuracy condition*/
/* Deprecated can be useful in the future */

bool rtk_fix_accuracy_condition(rtk_fix_t rtk_fix){
    if(rtk_fix_is_recent() && (rtk_fix == AUTONOMOUS || rtk_fix == DIFFERENTIAL || rtk_fix == RTK_FLOAT))
    {
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
    double local_plant_error_threshold = DEFAULT_PLANT_ERROR;
    // Check if it is the first sample, sets the variable
    bool is_first_sample = first_sample_check(gps_tracker);

    local_delta_calculated = calculate_accumulated_distance(gps_tracker->anchor_position, gps_tracker->current_position, is_first_sample);
    ESP_LOGI("State Machine Loop", "Current delta calculated: %.9f", local_delta_calculated);

    // If the data has RTK Corrections confirmed then the threshold can be set to the minimum value

    if(gps_tracker->current_position.RTK_fix == RTK_FIXED && rtk_fix_is_recent())
    {
        local_plant_error_threshold = RTK_MODE_PLANT_ERROR;
    }
    else
    {
        local_plant_error_threshold = DEFAULT_PLANT_ERROR;
    }

    if (local_delta_calculated > local_plant_error_threshold && local_delta_calculated < HIGH_SIDE_MOVEMENT_ERROR)
    {
        gps_tracker->accumulated_distance = gps_tracker->accumulated_distance + local_delta_calculated;
        gps_tracker->anchor_position.latitude = gps_tracker->current_position.latitude;
        gps_tracker->anchor_position.longitude = gps_tracker->current_position.longitude;
        gps_tracker->anchor_position.altitude = gps_tracker->current_position.altitude;

        ESP_LOGI("State Machine Loop", "Accumulated distance is %f // Delta is %f //", gps_tracker->accumulated_distance, sys_data_copy.delta_pos);

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
        //gps_tracker->anchor_position = gps_tracker->current_position; // Reset anchor position to current position when entering IDLE
        gps_tracker->first_sample = true; // Reset first sample flag when entering IDLE
        *num_plantings_crop_row = 0;                                  // Reset the number of plantings in the current crop row when entering IDLE
        ESP_LOGI("State Machine Loop", "IDLE STATE");
        break;

    default:
        ESP_LOGW("state machine code", "ERROR ENTERED DEFAULT STATE");
        break;
    }
}
