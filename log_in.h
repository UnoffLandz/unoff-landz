#ifndef LOG_IN_H_INCLUDED
#define LOG_IN_H_INCLUDED

enum { //return values for validate_password function
    PASSWORD_CORRECT=0,
    PASSWORD_INCORRECT=-1
};

/** RESULT  : processes a log in request from the client

    RETURNS : void

    PURPOSE : reduce need for code in client_protocol_handler.c
*/
void process_log_in(int connection, unsigned char *data);


#endif // LOG_IN_H_INCLUDED
