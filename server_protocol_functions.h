#ifndef SERVER_PROTOCOL_FUNCTIONS_H_INCLUDED
#define SERVER_PROTOCOL_FUNCTIONS_H_INCLUDED

#include <stdlib.h>

/** RESULT  : sends the log_in_ok packet to the client

    RETURNS : void

    PURPOSE : to let the client know that char has been successfully logged in
*/
void send_login_ok(int connection);


/** RESULT  : sends the log_in_not_ok packet to the client

    RETURNS : void

    PURPOSE : to let the client know that char has not been successfully logged in
*/
void send_login_not_ok(int connection);


/** RESULT  : sends the you_dont_exist packet to the client

    RETURNS : void

    PURPOSE : to let the client know that char does not exist
*/
void send_you_dont_exist(int connection);


/** RESULT  : sends the create_char_ok packet to the client

    RETURNS : void

    PURPOSE : sent following successful char creation
*/
void send_create_char_ok(int sock);


/** RESULT  : sends the create_char_not_ok packet to the client

    RETURNS : void

    PURPOSE : sent following unsuccessful char creation
*/
void send_create_char_not_ok(int sock);


/** RESULT  : sends the you_are packet to the client

    RETURNS : void

    PURPOSE : sends the client a code to identify connection following log in
*/
void send_you_are(int connection);


/** RESULT  : sends the raw_text packet to client

    RETURNS : void

    PURPOSE : sends text messages to connected clients via the specified channel

    NOTES   :
*/
void send_raw_text(int connection, int chan_type, char *text);


/** RESULT  : sends the here_your_inventory packet to client

    RETURNS : void

    PURPOSE : sends the character inventory to client at log-in

    NOTES   :
*/
void send_here_your_inventory(int connection);


/** RESULT  : sends the get_active_channels packet to client

    RETURNS : void

    PURPOSE : sends active chat channels to client at log-in

    NOTES   :
*/
void send_get_active_channels(int connection);


/** RESULT  : sends the here_your_stats packet to client

    RETURNS : void

    PURPOSE : sends the characters stats to client at log-in

    NOTES   :
*/
void send_here_your_stats(int connection);


/** RESULT  : sends the change_map packet to client

    RETURNS : void

    PURPOSE : informs the client of the map to be used with this character

    NOTES   :
*/
void send_change_map(int connection, char *elm_filename);


/** RESULT  : creates the add_new_enhanced_actor_packet to client

    RETURNS : void

    PURPOSE :

    NOTES   : used by broadcast_add_new_enhanced_actor_packet function
*/
void add_new_enhanced_actor_packet(int connection, unsigned char *packet, int *packet_length);


/** RESULT  : creates the remove actor_packet to client

    RETURNS : void

    PURPOSE :

    NOTES   : used by broadcast_remove_actor_packet function
*/
void remove_actor_packet(int connection, unsigned char *packet, int *packet_length);


/** RESULT  : creates the add_actor_packet

    RETURNS : void

    PURPOSE :

    NOTES   : used by broadcast_actor_packet function
*/
void add_actor_packet(int connection, unsigned char move, unsigned char *packet, int *packet_length);


/** RESULT  : sends the new_minute packet

    RETURNS : void

    PURPOSE :

    NOTES   : used to set client game time
*/
void send_new_minute(int connection, int16_t minute);


/** RESULT  : sends the active channels packet

    RETURNS : void

    PURPOSE :

    NOTES   : used at log in
*/
void send_get_active_channels(int connection);

#endif // SERVER_PROTOCOL_FUNCTIONS_H_INCLUDED
