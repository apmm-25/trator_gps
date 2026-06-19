/* 
    Author: António Malato
    This is a header file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    


*/
#include "serial_comms.h"

#ifndef GPS_H
#define GPS_H

/*      STRUCTS     */
typedef struct {
    float latitude;
    float longitude;
    float altitude;
} GPSData;
#endif /* GPS_H */

