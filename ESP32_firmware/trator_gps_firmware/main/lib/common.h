/* 
    Author: António Malato
    This is a header file for common functions used across the project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    


*/

#

#ifndef COMMON_H
#define COMMON_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"


typedef enum {
    SERIAL_COMMS_OK = 0,
    SERIAL_COMMS_ERROR = -1,
    SERIAL_COMMS_TIMEOUT = -2,
    SERIAL_COMMS_INVALID_PARAM = -3
} serial_comms_status_t;

typedef enum {
    SERIAL_NOT_SET = 0,
    SERIAL_SET = 1
} serial_comm_set_status_t;

typedef enum {
    SERIAL_UART0 = 0,
    SERIAL_UART2 = 1,
    SERIAL_I2C = 2,
    SERIAL_SPI = 3
} serial_comms_type_t;

typedef struct {
   serial_comms_type_t comms_type; 
   uint32_t baudrate;
   uint8_t parity;
   uint8_t stop_bits;
   serial_comm_set_status_t set_status;
} serial_comms_config_t;


#endif /* COMMON_H */