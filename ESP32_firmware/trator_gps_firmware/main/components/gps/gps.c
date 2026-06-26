/* 
    Author: António Malato
    This is a c file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    


*/
#include "gps.h"

zedf9p_incoming_data_t parser_nmea_msg(){
    zedf9p_incoming_data_t parsed_data = {
        .altitude = 0,
        .checksum = 0,
        .latitude = 0,
        .longitude = 0,
        .satellite_number = 0,
        .timestamp = 0, 
    };
    uint8_t rx_buffer[256];
    uint8_t next_byte;

    if(serial_comms_receive_gps_data(&next_byte) == SERIAL_COMMS_OK){
        // check if its the end of the transmission etc
        // implement it with a READ from NMEA protocol read
    } 

    return parsed_data;
}

GPSData get_gps_data(){

    GPSData ret;
    ret.altitude = 0;
    ret.latitude = 0;
    ret.longitude = 0;

    zedf9p_incoming_data_t received_data;


    return ret;

}

uint16_t linear_distance_calculation(){
    // Calculate the linear distance between two LLA points
    return 16;
}

uint16_t calculate_accumulated_distance(GPSData last_pos, GPSData curr_pos){
    // calculation between two positions using the linear_distance_calculation

    // return the value to be incremented in the accumulated_distance gps_tracker_t type variable
    return 1;
}
