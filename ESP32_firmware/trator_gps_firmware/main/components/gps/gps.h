/* 
    Author: António Malato
    This is a header file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    


*/
#include "common.h"

#ifndef GPS_H
#define GPS_H


void parser_nmea_msg();

GPSData get_gps_data();

uint16_t linear_distance_calculation();

uint16_t calculate_accumulated_distance(GPSData last_pos, GPSData curr_pos);



#endif /* GPS_H */

