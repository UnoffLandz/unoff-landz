#ifndef MOTD_H_INCLUDED
#define MOTD_H_INCLUDED

#define MOTD_FILE "motd.msg"
#define SERVER_WELCOME_MSG "\nWELCOME TO THE UNOFF SERVER\n------------------------------------------------------------"

/** RESULT  : send motd.msg to socket 'connection'

    RETURNS : 0=success/-1=fail

    PURPOSE : send MOTD to connecting clients

    USAGE   : main.c accept_client

    NOTES   :
*/
void send_motd(int connection);


/** RESULT  : sends motd header to socket 'connection'

    RETURNS : void

    PURPOSE : include relevant server stats in MOTD (used at log in and protocol 232

    USAGE   : motd.c send_motd / protocol.c process_packet

    NOTES   :
*/
void send_motd_header(int connection);


/** RESULT  : sends text file contents to socket 'connection'

    RETURNS : 0=success/-1=fail

    PURPOSE : include text file content in MOTD

    USAGE   : files.c log_event / hash_commands.c process_hash_command

    NOTES   :
*/
int send_motd_file(int connection);

#endif // MOTD_H_INCLUDED
