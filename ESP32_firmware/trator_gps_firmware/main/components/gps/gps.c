/*
    Author: António Malato
    This is a c file for GPS functions of the tractor project

    If a change is made add the day and the title of the change here:

    18/6/2026 - Initial creation of the file
    27/6/2026 - Added gps data data receiving and parsing
    29/6/2026 - Added checksum verification and overall gns type parsing


*/
#include "gps.h"

float char_to_number_extraction(char *sub_string, int len)
{
    float ret_val = 0;
    float scale = 1;
    bool decimal = false;
    ESP_LOGW("CHAR_TO_FLOAT_CONV", "Here is the sub_string: %s", sub_string);

    for (size_t i = 0; i < len; i++)
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

        ESP_LOGW("CHAR_TO_FLOAT_CONV", "Here is the float value: %f, Iteration: %d", ret_val, i);
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

bool zedf9p_data_receiver(nmea_raw_data_struct *data)
{
    uint8_t rx_byte;

    if (serial_comms_receive_gps_data(&rx_byte) == SERIAL_COMMS_OK)
    {
        // check if its the end of the transmission etc
        // implement it with a READ from NMEA protocol read
        if (rx_byte == GPS_LINE_TERMINATOR)
        {
            data->rx_buffer[data->index - 1] = '\0';
            data->line_len = data->index - 1;
            data->index = 0;
            return true;
        }
        else
        {
            data->rx_buffer[data->index] = rx_byte;
            data->index++;
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
        index++;
    }

    if (data->rx_buffer[index] == GPS_CHECKSUM_FIELD_INDICATOR)
    {

        tx_checksum = hex_to_byte(data->rx_buffer[(index + 1)], data->rx_buffer[(index + 2)]);
        ESP_LOGI("NMEA_CHECKSUM_COMP", "Checksum val: %X", tx_checksum);
        if (tx_checksum == rx_checksum)
        {
            ESP_LOGW("NMEA_CHECKSUM_COMP", "CHECKSUM VALIDATED");
            return true;
        }
        else
        {
            ESP_LOGW("NMEA_CHECKSUM_COMP", "CHECKSUM REJECTED");

            return false;
        }
    }
    else
    {

        ESP_LOGW("NMEA CHECKSUM CHECK", "REACHED END OF LINE WITHOUT FINDING CHECKSUM FIELD");
        return false;
    }
}

float nmea_float_parser(nmea_raw_data_struct *data)
{
    int inside_field_byte_index = 0;
    char sub_buffer[GPS_DATA_FIELD_MAX_STR_LEN];
    float ret;
    
    // When it enters it is in the initial ',' he catches
    data->parser_index++;
    while ((data->rx_buffer[data->parser_index]) != GPS_DATA_FIELD_TERMINATOR)
    {
        sub_buffer[inside_field_byte_index] = data->rx_buffer[data->parser_index];
        data->parser_index++;
        inside_field_byte_index++;
    }
    sub_buffer[inside_field_byte_index] = '\0';

    ret = char_to_number_extraction(sub_buffer, strlen(sub_buffer));
    data->parser_index--;
    return ret;
}

uint8_t nmea_int_parser(nmea_raw_data_struct *data)
{
    int inside_field_byte_index = 0;
    char sub_buffer[GPS_DATA_FIELD_MAX_STR_LEN];
    float ret;
    data->parser_index++;
    while ((data->rx_buffer[data->parser_index]) != GPS_DATA_FIELD_TERMINATOR)
    {
        sub_buffer[inside_field_byte_index] = data->rx_buffer[data->parser_index];
        data->parser_index++;
        inside_field_byte_index++;
    }
    sub_buffer[inside_field_byte_index] = '\0';
    ret = string_to_int(sub_buffer);
    return ret;
}

zedf9p_incoming_data_t GNS_parser(nmea_raw_data_struct *data)
{
    int msg_field_index = 0;
    zedf9p_incoming_data_t ret;
    data->parser_index = 0;

    ESP_LOGI("GNS_parser", "phrase to be parsed: %s", (data->rx_buffer));

    while ((data->rx_buffer[data->parser_index]) != '\0')
    {
        if ((data->rx_buffer[data->parser_index]) == GPS_DATA_INITIAL_CHAR)
        {
            //$ começa os dados
            msg_field_index = 0;

        }
        else if ((data->rx_buffer[data->parser_index]) == GPS_DATA_FIELD_TERMINATOR)
        {
            
            msg_field_index++;
            // virgula comeca o field
            if ((data->rx_buffer[(data->parser_index) + 1]) == GPS_DATA_FIELD_TERMINATOR)
            {
                // the field is empty
                msg_field_index++;
            }
            else
            {
                
                switch (msg_field_index)
                {

                case TIME:
                    double timestamp_ = (double)nmea_float_parser(data);
                    ret.timestamp = timestamp_;
                    break;

                case LAT:
                    ret.latitude = (double)nmea_float_parser(data);
                    break;

                case NS:
                    ret.NS = data->rx_buffer[data->parser_index];
                    break;

                case LON:
                    ret.longitude = (double)nmea_float_parser(data);
                    break;

                case EW:
                    ret.EW = data->rx_buffer[data->parser_index];
                    break;

                case NUMSV:
                    ret.satellite_number = nmea_int_parser(data);
                    break;

                case ALT:
                    ret.altitude = (double)nmea_float_parser(data);
                    break;

                default:

                    break;
                }
            }
        }
        data->parser_index++;
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
