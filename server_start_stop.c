#include <stdio.h> //supports printf function
#include <stdlib.h> //supports exit function and EXIT_FAILURE definition

#include "date_time_functions.h"
#include "logging.h"

void stop_server(){

    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    time_t stop_time=time(NULL);

    get_time_stamp_str(stop_time, time_stamp_str);
    get_verbose_date_str(stop_time, verbose_date_stamp_str);

    printf("SERVER STOP at %s on %s\n", time_stamp_str, verbose_date_stamp_str);
    printf("check 'error.log' for details\n");

    log_event(EVENT_ERROR, "SERVER STOP at %s on %s\n", time_stamp_str, verbose_date_stamp_str);

    exit(EXIT_FAILURE);
}
