
/* 
    Author: António Malato
    This is a header file that defines project data types used across the project
    Structures are defined here
    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    


*/

#ifndef DAT_H
#define DAT_H


#include "libs.h"

typedef struct {
   serial_comms_type_t comms_type; 
   uint32_t baudrate;
   uint8_t parity;
   uint8_t stop_bits;
   serial_comm_set_status_t set_status;
} serial_comms_config_t;

typedef struct {
    bool first_sample;
    uint16_t accumulated_distance;
} gps_tracker_t;

typedef struct {
    float latitude;
    float longitude;
    float altitude;
} GPSData;

typedef struct {
    bool reset;
    uint8_t delta_pos;
    uint8_t plant_time;

} web_cmds_t; // Structure that holds incomming commands and changes to constants incoming from the web app

typedef struct {
    uint8_t delta_pos;
    uint16_t plant_time;

} sys_data_t; // Structure that holds system data to be used across the project

typedef struct {
    bool plant_mode_active;
    GPSData gps_data;
} web_data_t; // Structure that holds data to be sent to the web interface to show the user

typedef struct {
    state_machine_state_t next_state;
    plant_mode_substate_t next_sub_state;
} state_machine_data_t;

typedef struct
{
    button_states_t current_state;
    int64_t last_pressed_timestamp;
} button_t; // Tracks the button and the system state

#endif /* DAT_H */