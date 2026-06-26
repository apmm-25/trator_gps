/* 
    Author: António Malato
    This is a c file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    


*/
#include "gps.h"


void parser_nmea_msg(){
    
}

GPSData get_gps_data(){

    GPSData ret;
    ret.altitude = 0;
    ret.latitude = 0;
    ret.longitude = 0;

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
