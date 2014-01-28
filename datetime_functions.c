#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void get_time_stamp_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_time;

    // Convert to struct in gmt representation.
    cooked_time = gmtime (&raw_time);

    strftime (buffer, 9, "%H:%m:%S", cooked_time);
}

void get_date_stamp_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_date;

    // Convert to struct in gmt representation.
    cooked_date = gmtime (&raw_time);

    strftime (buffer, 11, "%d:%m:%C%y", cooked_date);
}

void get_verbose_date_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_date;

    // Convert to struct in gmt representation.
    cooked_date = gmtime (&raw_time);

    strftime (buffer, 50, "%A %d %B %C%y", cooked_date);
}

void get_time_up_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_date;

    // Convert to struct in gmt representation.
    cooked_date = gmtime (&raw_time);

    strftime (buffer, 15, "%H hrs %m mins", cooked_date);

}
