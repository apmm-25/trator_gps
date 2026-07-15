/*
    Author: António Malato
    This is a c file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    27/6/2026 - Added gps data data receiving and parsing
    29/6/2026 - Added checksum verification and overall gns type parsing


*/
#include "gps.h"

double char_to_number_extraction(char *sub_string)
{
    double ret_val = 0;
    double scale = 1;
    bool decimal = false;
    // ESP_LOGW("CHAR_TO_FLOAT_CONV", "Here is the sub_string: %s", sub_string);
    int len = strlen(sub_string);
    for (int i = 0; i < len; i++)
    {
        if (sub_string[i] == '.')
        {
            decimal = true;
        }
        else
        {
            if (!decimal)
            {
                ret_val = ret_val * 10 + (sub_string[i] - '0');
            }
            else
            {
                scale *= 10;
                ret_val = ret_val + ((sub_string[i] - '0') / scale);
            }
        }

        // ESP_LOGW("CHAR_TO_FLOAT_CONV", "Here is the float value: %f, Iteration: %d", ret_val, i);
    }

    return ret_val;
}

uint8_t hex_to_byte(char high, char low)
{
    // and with 0XDF converts the ascii letter into uppercase
    uint8_t h = (high > '9') ? (high & 0xDF) - 'A' + 10 : high - '0';
    uint8_t l = (low > '9') ? (low & 0xDF) - 'A' + 10 : low - '0';
    return (h << 4) | l;
}

uint8_t string_to_int(char *num)
{
    uint8_t ret_val = 0;
    for (size_t i = 0; i < strlen(num); i++)
    {
        ret_val = ret_val * 10 + (num[i] - '0');
    }

    return ret_val;
}

bool zedf9p_data_receiver(nmea_raw_data_struct *data, bool *receiving)
{
    uint8_t rx_byte;

    if (uart_read_bytes(GPS_UART, &rx_byte, sizeof(rx_byte), portMAX_DELAY) == 1)
    {
        // check if its the end of the transmission etc
        // implement it with a READ from NMEA protocol read
        // ESP_LOGI("UART receiver", "there is the byte received %c", (char)rx_byte);

        if (rx_byte == GPS_BEGIN_RECEIVE)
        {
            *receiving = true;
        }

        if (*receiving && data->index < MAX_BUFFER_SIZE)
        {
            if (rx_byte == GPS_LINE_TERMINATOR)
            {
                data->rx_buffer[data->index] = '\0';
                data->line_len = strlen(data->rx_buffer);
                data->index = 0;
                // ESP_LOGI("uart receiver", "The final string received is: %s", data->rx_buffer);
                *receiving = false;
                return true;
            }
            else if (rx_byte == '\r')
            {
                // ignore
            }
            else
            {
                data->rx_buffer[data->index] = rx_byte;
                data->index++;
            }
        }
    }

    return false;
}

uint8_t nmea_checksum_computation(const char *sentence)
{
    // Skip leading '$' if present
    if (*sentence == GPS_DATA_INITIAL_CHAR)
        sentence++;

    uint8_t checksum = 0;
    while (*sentence && *sentence != GPS_CHECKSUM_FIELD_INDICATOR)
    {
        checksum ^= (uint8_t)*sentence++;
    }
    return checksum;
}

bool nmea_checksum_comparison(const nmea_raw_data_struct *data)
{
    uint8_t rx_checksum = nmea_checksum_computation(data->rx_buffer);
    uint8_t index = 0;
    uint8_t tx_checksum;

    // check either we reached the checksum, or if in case of the string comes with no checksum, the string end
    while ((data->rx_buffer[index]) != GPS_CHECKSUM_FIELD_INDICATOR && (data->rx_buffer[index]) != '\0')
    {
        if (index == data->line_len)
        {
            return false;
        }

        index++;
        // ESP_LOGI("NMEA_CHECKSUM_COMP", "Parsing the char : %c", data->rx_buffer[index]);
    }

    if (data->rx_buffer[index] == GPS_CHECKSUM_FIELD_INDICATOR)
    {

        tx_checksum = hex_to_byte(data->rx_buffer[(index + 1)], data->rx_buffer[(index + 2)]);
        if (tx_checksum == rx_checksum)
        {
            // ESP_LOGW("NMEA_CHECKSUM_COMP", "CHECKSUM VALIDATED");
            return true;
        }
        else
        {
            // ESP_LOGW("NMEA_CHECKSUM_COMP", "CHECKSUM REJECTED");

            return false;
        }
    }
    else
    {

        // ESP_LOGW("NMEA CHECKSUM CHECK", "REACHED END OF LINE WITHOUT FINDING CHECKSUM FIELD");
        return false;
    }
}

double nmea_double_parser(char *field_data)
{
    double ret;
    ret = char_to_number_extraction(field_data);
    return ret;
}

uint8_t nmea_int_parser(char *field_data)
{
    uint8_t ret;
    ret = string_to_int(field_data);
    return ret;
}

// TO DO: REFACTOR TO MAKE IT MORE PRACTICAL TO ADD NEW SWITCHES FOR NEW MESSAGES
//        PUT THE SWITCHES INSIDE INDIVIDUAL FUNCTIONS
zedf9p_incoming_data_t nmea_gps_type_loop(nmea_raw_data_struct *data, gps_msg_t gps_msg_type)
{

    int msg_field_index = 0;
    zedf9p_incoming_data_t ret = {0};
    data->parser_index = 0;
    int sub_buffer_index = 0;
    char sub_buffer[GPS_DATA_FIELD_MAX_STR_LEN];
    int cpy_index = 0; // copy of the latest index as reference for the sub_buffer creation

    while ((data->rx_buffer[data->parser_index]) != '\0' && (data->rx_buffer[data->parser_index]) != GPS_CHECKSUM_FIELD_INDICATOR)
    {

        if ((data->rx_buffer[data->parser_index]) == GPS_DATA_INITIAL_CHAR)
        {
            //$ começa os dados
            msg_field_index = 0;
            while ((data->rx_buffer[data->parser_index]) != GPS_DATA_FIELD_TERMINATOR)
            {
                data->parser_index++;
            }
        }
        else if ((data->rx_buffer[data->parser_index]) == GPS_DATA_FIELD_TERMINATOR)
        {

            msg_field_index++;
            // virgula comeca o field
            data->parser_index++; // increment the index to point to the first char of the field
            sub_buffer_index = 0;
            cpy_index = data->parser_index;
            while ((data->rx_buffer[cpy_index]) != GPS_DATA_FIELD_TERMINATOR && (data->rx_buffer[cpy_index]) != GPS_CHECKSUM_FIELD_INDICATOR)
            {
                // copy the field to a sub buffer to be parsed
                sub_buffer[sub_buffer_index] = data->rx_buffer[cpy_index];
                sub_buffer_index++;
                cpy_index++;
            }
            sub_buffer[sub_buffer_index] = '\0';
            data->parser_index = cpy_index;
            if (sub_buffer_index == 0)
            {
                continue; // Skip processing for empty fields
            }
            else if (sub_buffer_index > 0 && gps_msg_type == GNS)
            {
                switch (msg_field_index)
                {

                case GNS_TIME:
                    double timestamp_ = nmea_double_parser(sub_buffer);
                    ret.timestamp = timestamp_;
                    break;

                case GNS_LAT:
                    ret.latitude = nmea_double_parser(sub_buffer);
                    break;

                case GNS_NS:
                    ret.NS = sub_buffer[0];
                    break;

                case GNS_LON:
                    ret.longitude = nmea_double_parser(sub_buffer);
                    break;

                case GNS_EW:
                    ret.EW = sub_buffer[0];
                    break;

                case GNS_NUMSV:
                    ret.satellite_number = nmea_int_parser(sub_buffer);
                    break;

                case GNS_ALT:
                    ret.altitude = nmea_double_parser(sub_buffer);
                    break;
                case GNS_HDOP:
                    ret.HDOP = nmea_double_parser(sub_buffer);
                    break;

                default:

                    break;
                }
            }
            else if (sub_buffer_index > 0 && gps_msg_type == PUBX)
            {
                // Implement PUBX parsing logic here
                switch (msg_field_index)
                {
                // Add PUBX-specific field parsing cases here
                case PUBX_TIME:
                    double timestamp_ = nmea_double_parser(sub_buffer);
                    ret.timestamp = timestamp_;
                    break;
                case PUBX_LAT:
                    ret.latitude = nmea_double_parser(sub_buffer);
                    break;
                case PUBX_NS:
                    ret.NS = sub_buffer[0];
                    break;
                case PUBX_LON:
                    ret.longitude = nmea_double_parser(sub_buffer);
                    break;
                case PUBX_EW:
                    ret.EW = sub_buffer[0];
                    break;
                case PUBX_NUMSV:
                    ret.satellite_number = nmea_int_parser(sub_buffer);
                    break;
                case PUBX_ALTREF:
                    ret.altitude = nmea_double_parser(sub_buffer);
                    break;
                case PUBX_HDOP:
                    ret.HDOP = nmea_double_parser(sub_buffer);
                    break;
                default:
                    break;
                }
                // ESP_LOGI("PUBX_parser", "Parsed PUBX field index: %d, value: %s", msg_field_index, sub_buffer);
            }
            else if (sub_buffer_index > 0 && gps_msg_type == GGA)
            {
                switch (msg_field_index)
                {
                case GGA_TIME:
                    double timestamp_ = nmea_double_parser(sub_buffer);
                    ret.timestamp = timestamp_;
                    break;
                case GGA_LAT:
                    ret.latitude = nmea_double_parser(sub_buffer);
                    break;
                case GGA_NS:
                    ret.NS = sub_buffer[0];
                    break;
                case GGA_LON:
                    ret.longitude = nmea_double_parser(sub_buffer);
                    break;
                case GGA_EW:
                    ret.EW = sub_buffer[0];
                    break;
                case GGA_NUMSV:
                    ret.satellite_number = nmea_int_parser(sub_buffer);
                    break;
                case GGA_ALT:
                    ret.altitude = nmea_double_parser(sub_buffer);
                    break;
                case GGA_HDOP:
                    ret.HDOP = nmea_double_parser(sub_buffer);
                    break;
                default:
                    break;
                }
            }
            else if (sub_buffer_index > 0 && gps_msg_type == GLL)
            {
                switch (msg_field_index)
                {
                case GLL_TIME:
                    double timestamp_ = nmea_double_parser(sub_buffer);
                    ret.timestamp = timestamp_;
                    break;
                case GLL_LAT:
                    ret.latitude = nmea_double_parser(sub_buffer);
                    break;
                case GLL_NS:
                    ret.NS = sub_buffer[0];
                    break;
                case GLL_LON:
                    ret.longitude = nmea_double_parser(sub_buffer);
                    break;
                case GLL_EW:
                    ret.EW = sub_buffer[0];
                    break;
                default:
                    break;
                }
            }
        }
    }
    return ret;
}

gps_msg_t check_gps_type(char *data_buffer)
{
    gps_msg_t ret;

    if (*(data_buffer) == GPS_DATA_INITIAL_CHAR)
    {
        if (strstr(data_buffer, PUBX_MSG_TYPE_ID))
        {
            ret = PUBX;
        }
        else if (strstr(data_buffer, GNS_MSG_TYPE_ID))
        {

            ret = GNS;
        }
        else if (strstr(data_buffer, GGA_MSG_TYPE_ID))
        {
            ret = GGA;
        }
        else if (strstr(data_buffer, GLL_MSG_TYPE_ID))
        {
            ret = UNDEFINED;
        }
        else
        {
            ret = UNDEFINED;
        }
        return ret;
    }
    else
    {
        ESP_LOGW("GPS TYPE", "INVALID GPS MSG, MISSING $");
        ret = UNDEFINED;
        return ret;
    }
}

bool gps_data_validity(GPSData ret_data)
{
    if (ret_data.HDOP > HDOP_MAX_LIMIT)
    {
        return false;
    }
    else if (ret_data.latitude == 0.0 && ret_data.altitude == 0.0 && ret_data.longitude == 0.0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
/* Passes a copy of the extracted gps data and transforms into actual decimal degrees lat/lon/altitude data */
bool transform_into_decimal_degrees(zedf9p_incoming_data_t data, GPSData *ret)
{

    ret->HDOP = data.HDOP;
    ret->satellite_number = data.satellite_number;

    double lat_degrees = floor((data.latitude / 100));
    double lon_degrees = floor((data.longitude / 100));

    double lat_minutes = data.latitude - (lat_degrees * 100);
    double lon_minutes = data.longitude - (lon_degrees * 100.0);

    ret->latitude = lat_degrees + (lat_minutes / 60.0);
    ret->longitude = lon_degrees + (lon_minutes / 60.0);
    ret->altitude = data.altitude;

    if (data.NS != 'S' && data.NS != 'N')
    {
        return false;
    }
    else if (data.EW != 'E' && data.EW != 'W')
    {
        return false;
    }
    else
    {
        ret->latitude = (data.NS == 'S') ? -ret->latitude : ret->latitude;
        ret->longitude = (data.EW == 'W') ? -ret->longitude : ret->longitude;
    }

    if (gps_data_validity(*ret))
    {
        return true;
    }
    else
    {
        return false;
    }
}

double calculate_accumulated_distance(GPSData last_pos, GPSData curr_pos, bool first_sample)
{
    //ESP_LOGI("ACC distance", "Last Pos LAT: %f // LON: %f // ALT: %f", last_pos.latitude, last_pos.longitude, last_pos.altitude);
    //ESP_LOGI("ACC distance", "Current Pos LAT: %f // LON: %f // ALT: %f", curr_pos.latitude, curr_pos.longitude, curr_pos.altitude);
    //ESP_LOGI("ACC distance", "First Sample Value is: %d", first_sample);

    if (first_sample)
    {
        return 0.0;
    }

    if (curr_pos.HDOP > HDOP_MAX_LIMIT)
    {
        return 0.0;
    }

    if (!gps_data_validity(last_pos) || !gps_data_validity(curr_pos))
    {
        return 0.0;
    }

    double lat_diff = curr_pos.latitude - last_pos.latitude;
    double lon_diff = curr_pos.longitude - last_pos.longitude;
    double alt_diff = curr_pos.altitude - last_pos.altitude;

    
    ESP_LOGI("ACC distance", "Delta LAT: %f // LON: %f // ALT: %f", lat_diff, lon_diff, alt_diff);

    if (fabs(lat_diff) < 1e-9 && fabs(lon_diff) < 1e-9 && fabs(alt_diff) < 1e-9)
    {
        return 0.0;
    }

    double lat1r = last_pos.latitude * M_PI / 180.0;
    double lat2r = curr_pos.latitude * M_PI / 180.0;
    double dlat = lat_diff * M_PI / 180.0;
    double dlon = lon_diff * M_PI / 180.0;

    double dx = dlon * cos((lat1r + lat2r) * 0.5) * EARTH_RADIUS;
    double dy = dlat * EARTH_RADIUS;
    double dz = alt_diff;

    return sqrt(dx * dx + dy * dy);
}