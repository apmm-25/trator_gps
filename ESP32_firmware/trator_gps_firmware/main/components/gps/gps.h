/* 
    Author: António Malato
    This is a header file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    


*/


#ifndef GPS_H
#define GPS_H

#include "common.h"


uint16_t linear_distance_calculation();

uint16_t calculate_accumulated_distance(GPSData last_pos, GPSData curr_pos);

bool zedf9p_data_receiver(nmea_raw_data_struct* data);

zedf9p_incoming_data_t PUBX_parser(nmea_raw_data_struct* data);

zedf9p_incoming_data_t GNS_parser(nmea_raw_data_struct* data);

bool nmea_checksum_comparison(const nmea_raw_data_struct* data);

gps_msg_t check_gps_type(char *data_buffer);

zedf9p_incoming_data_t nmea_gps_type_loop(nmea_raw_data_struct *data, gps_msg_t gps_msg_type);

#endif /* GPS_H */

