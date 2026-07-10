
/* 
    Author: António Malato
    This is a header file that defines projects enumerated types
    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    1/7/2026 - Extended to parse PUBX messages
    9/7/2926 - Extended to parse GGA messages
    


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
    GGA = 2,
    GLL = 3,
} gps_msg_t;

typedef enum{
    GNS_ID = 0,
    GNS_TIME = 1,
    GNS_LAT = 2,
    GNS_NS = 3,
    GNS_LON = 4,
    GNS_EW = 5,
    GNS_POSMODE = 6,
    GNS_NUMSV = 7,
    GNS_HDOP = 8, 
    GNS_ALT = 9,
    GNS_SEP = 10,
    GNS_DIFFAGE = 11,
    GNS_DIFFSTATION = 12,
    GNS_NAVSTATUS = 13,
} gns_data_fields_t;


typedef enum{
    PUBX_ID = 0,
    PUBX_MSG_ID = 1,
    PUBX_TIME = 2,
    PUBX_LAT = 3,
    PUBX_NS = 4,
    PUBX_LON = 5,
    PUBX_EW = 6,
    PUBX_ALTREF = 7,
    PUBX_NAVSTATUS = 8,
    PUBX_HACC = 9,
    PUBX_VACC = 10,
    PUBX_SOG = 11,
    PUBX_COG = 12,
    PUBX_VVEL = 13,
    PUBX_DIFFAGE = 14,
    PUBX_HDOP = 15,
    PUBX_VDOP = 16,
    PUBX_TDOP = 17,
    PUBX_NUMSV = 18,
    PUBX_RESERVED = 19,
    PUBX_DR = 20,
} pubx_data_fields_t;


typedef enum {
    GGA_ID = 0,
    GGA_TIME = 1,
    GGA_LAT = 2,
    GGA_NS = 3,
    GGA_LON = 4,
    GGA_EW = 5,
    GGA_QUALITY = 6,
    GGA_NUMSV = 7,
    GGA_HDOP = 8, 
    GGA_ALT = 9,
    GGA_ALTUNIT = 10,
    GGA_SEP = 11,
    GGA_SEPUNIT = 12,
    GGA_DIFFAGE = 13,
    GGA_DIFFSTATION = 14,

} gga_data_fields_t;

typedef enum {
    GLL_ID = 0,
    GLL_LAT = 1,
    GLL_NS = 2,
    GLL_LON = 3,
    GLL_EW = 4,
    GLL_TIME = 5,
    GLL_STAT = 6,
    GLL_POSMODE = 7,
} gll_data_fields_t;


#endif /* TYPES_H */