
/* 
    Author: António Malato
    This is a header file that defines projects enumerated types
    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    


*/

#ifndef TYPES_H
#define TYPES_H

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

typedef enum {
    IDLE = 0,
    PLANT_MODE = 1,
} state_machine_state_t;

typedef enum {
    IDLE_SUBSTATE = -1, // When the overall state is in IDLE
    WAIT_FOR_PLANT = 0,
    NORMAL_OPERATION = 1,
} plant_mode_substate_t;

typedef enum {
    PRESSED = 0,
    UNPRESSED = 1,
} button_states_t;

typedef enum {
    UNDEFINED = -1,
    GNS = 0,
    PUBX = 1,
} gps_msg_t;

typedef enum{
    GNS_ID = 0,
    TIME = 1,
    LAT = 2,
    NS = 3,
    LON = 4,
    EW = 5,
    POSMODE = 6,
    NUMSV = 7,
    HDOP = 8, 
    ALT = 9,
    SEP = 10,
    DIFFAGE = 11,
    DIFFSTATION = 12,
    NAVSTATUS = 13,
} gns_data_fields_t;
#endif /* TYPES_H */