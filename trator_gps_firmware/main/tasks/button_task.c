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
    yellow_led_t planting_time_led_indication;
    button_state.current_state = UNPRESSED;
    planting_time_led_indication.current_state = OFF;
    planting_time_led_indication.last_on_timestamp = 0;

    while (1)
    {

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

        EventBits_t bits = xEventGroupGetBits(system_events);

        if (bits & EVENT_YELLOW_LED)
        {
            if (planting_time_led_indication.current_state == OFF)
            {
                planting_time_led_indication.last_on_timestamp = esp_timer_get_time();
                planting_time_led_indication.current_state = ON;
                gpio_set_level(YELLOW_LED_PIN, true);
            }
        }
        else if (planting_time_led_indication.current_state == ON)
        {
            int64_t led_on_duration = (esp_timer_get_time() - planting_time_led_indication.last_on_timestamp) / 1000;
            if (led_on_duration >= YELLOW_LED_BLINK_TIME)
            {
                xEventGroupClearBits(system_events, EVENT_YELLOW_LED);
                planting_time_led_indication.current_state = OFF;
                gpio_set_level(YELLOW_LED_PIN, false);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
