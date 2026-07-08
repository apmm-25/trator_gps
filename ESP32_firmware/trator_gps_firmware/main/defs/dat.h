
/* 
    Author: António Malato
    This is a header file that defines project data types used across the project
    Structures are defined here
    If a change is made add the day and the title of the change here:

    24/6/2026 - Initial creation of the file
    1/7/2026 -  Changed the data types of the latitude, longitude and altitude to double to increase precision
    


*/

#ifndef DAT_H
#define DAT_H


#include "libs.h"

typedef struct {
    double latitude;
    double longitude;
    double altitude;
    double HDOP;
    uint16_t satellite_number;
} GPSData; // possivelmente inutil


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
    GPSData last_position;
    GPSData current_position;
} gps_tracker_t;

typedef struct {
    double latitude;
    double longitude;
    double altitude;
    double HDOP;
    char EW;
    char NS;
    double timestamp;
    uint16_t satellite_number;
    uint8_t checksum;
} zedf9p_incoming_data_t;

typedef struct {
    char rx_buffer[256];
    uint16_t index; //iterate through this to set the value while reading
    uint16_t parser_index; //iterate through this during parsing
    uint16_t line_len;
} nmea_raw_data_struct;

typedef struct {
    bool reset;
    uint8_t delta_pos;
    double allowed_delta_plant_error;
    uint8_t plant_time; // miliseconds

} web_cmds_t; // Structure that holds incomming commands and changes to constants incoming from the web app

typedef struct {
    uint8_t delta_pos;
    double allowed_delta_plant_error;
    uint16_t plant_time;
    
} sys_data_t; // Structure that holds system data to be used across the project

typedef struct {
    uint64_t numPlants;
    bool plant_mode_active;
    uint8_t current_acc_distance;
} plant_data_t;

typedef struct {
    plant_data_t plant_data;
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