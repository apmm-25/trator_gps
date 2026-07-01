/*
    Author: António Malato
    This is a c file for the button logic function of the tractor project
    It has the implementation of the button logic thread, which is responsible for managing the button state and triggering the appropriate actions based on the button press.

    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file

*/

#include "button_task.h"

static const char *TAG = "BUTTON_TASK";

void button_task_start(void)
{

    TaskHandle_t button_task_handle;
    xTaskCreate(button_task, "Button Task", 2048, NULL, 5, &button_task_handle);
    ESP_LOGI(TAG, "Button task created");
}

void button_task(void *pvParameters)
{

    ESP_LOGI(TAG, "Button task started");
    bool plant_mode_state = false; // THIS WILL KEEP UP WITH THE CHANGING STATES
    button_t button_state;
    button_state.current_state = UNPRESSED;
    while (1)
    {

        // Check button state and trigger actions based on the button press
        // For example, if the button is pressed, change the state of the state machine or trigger an event
        //ESP_LOGI(TAG, "Button task running");
         // Delay for 100 ms to avoid busy waiting
        if (!gpio_get_level(BUTTON_IN))
        {

            if (button_state.current_state == UNPRESSED)
            {
                button_state.last_pressed_timestamp = esp_timer_get_time();
                button_state.current_state = PRESSED;
            }
            else if (button_state.current_state == PRESSED)
            {
                int64_t held_time = (esp_timer_get_time() - button_state.last_pressed_timestamp) / 1000;

                if (held_time >= BUTTON_PRESS_TIME)
                {
                    plant_mode_state = !plant_mode_state;
                    button_state.current_state = UNPRESSED;
                }
            }
        }
        else
        {
            button_state.current_state = UNPRESSED;
        }

        if (plant_mode_state)
        {
            xEventGroupSetBits(system_events, EVENT_PLANT_MODE);
        }
        else
        {
            xEventGroupClearBits(system_events, EVENT_PLANT_MODE);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}