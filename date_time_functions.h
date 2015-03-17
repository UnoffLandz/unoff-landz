#ifndef DATE_TIME_FUNCTIONS_H_INCLUDED
#define DATE_TIME_FUNCTIONS_H_INCLUDED

#include <time.h>       // supports gettimeof day function (saves separately having to add to any module that calls functions from this module
#include <sys/time.h>   // supports time_t data type

struct timeval time_check;//output struct for gettimeofday function

/** RESULT  : gets a time stamp in format 'hh:mm:ss'

    RETURNS : void

    PURPOSE : stamp the log with the time of an event

    NOTES   : buffer must be 9 bytes long
*/
void get_time_stamp_str(time_t raw_time, char *buffer);


/** RESULT  : gets a date stamp in format 'dd:mm:yyyy'

    RETURNS : void

    PURPOSE : stamp the log with the current time

    NOTES   : buffer must be 11 bytes long
*/
void get_date_stamp_str(time_t raw_time, char *buffer);


/** RESULT  : gets a date stamp in format 'date month year'

    RETURNS : void

    PURPOSE : stamp the log with the current date

    NOTES   : buffer must be 11 bytes long
*/
void get_verbose_date_str(time_t raw_time, char *buffer);


/** RESULT  : gets a time string in format 'hrs mins' based on the difference between time values

    RETURNS : void

    PURPOSE : to return the time the server has been up on the motd header

    NOTES   : buffer must be 15 bytes long
*/
void get_time_up_str(time_t raw_time, char *buffer);

#endif // DATE_TIME_FUNCTIONS_H_INCLUDED
