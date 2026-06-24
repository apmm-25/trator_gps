/* 
    Author: António Malato
    This is a header file for serial communication functions across all projects I will ever work on 
    It is meant to be reusable and modular, so that I can easily include it in any project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file



*/


#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "common.h"




#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H


void serial_comms_init();

serial_comms_status_t serial_comms_send_data(const char* data, size_t length, serial_comms_type_t comms_type);

serial_comms_status_t serial_comms_receive_data(char* buffer, size_t buffer_length, size_t* received_length, serial_comms_type_t comms_type, uint32_t timeout_ms);

#endif /* SERIAL_COMMS_H */