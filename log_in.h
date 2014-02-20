#ifndef LOG_IN_H_INCLUDED
#define LOG_IN_H_INCLUDED

enum { //return values from is_char_concurrent function
    CHAR_NON_CONCURRENT=0,
    CHAR_CONCURRENT=-1
};

enum { //return values for validate_password function
    PASSWORD_CORRECT=0,
    PASSWORD_INCORRECT=-1
};

enum { //char status
    CHAR_ALIVE,
    CHAR_DEAD,
    CHAR_BANNED
};

int is_char_concurrent(int connection);

void send_login_ok(int connection);

void send_login_not_ok(int connection);

void send_you_dont_exist(int connection);

void send_you_are(int connection);


/** RESULT  : process client log in

    RETURNS : void

    PURPOSE : reduce size of protocol.c

    USAGE   : protocol.c process_packet
*/
void process_log_in(int connection, char *text);

#endif // LOG_IN_H_INCLUDED
