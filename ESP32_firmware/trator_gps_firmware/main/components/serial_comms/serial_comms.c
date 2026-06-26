/*
    Author: António Malato
    This is a file for serial communication functions across all projects I will ever work on
    It is meant to be reusable and modular, so that I can easily include it in any project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file



*/

#include "serial_comms.h"

void serial_comms_init()
{
    //GPS UART INITIALIZATION
     uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    
    ESP_ERROR_CHECK(uart_param_config(GPS_UART, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(GPS_UART, GPS_UART_TX, GPS_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    uart_driver_install(
    GPS_UART,
    4096,
    0,
    0,
    NULL,
    0);


}

serial_comms_status_t serial_comms_send_data(const char *data, size_t length, serial_comms_type_t comms_type);

serial_comms_status_t serial_comms_receive_data(char *buffer, size_t buffer_length, size_t *received_length, serial_comms_type_t comms_type, uint32_t timeout_ms);