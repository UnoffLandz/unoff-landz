#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

enum { /* client to server protocol */
    RAW_TEXT=0,
    MOVE_TO=1,
    SEND_PM=2,
    GET_PLAYER_INFO=5,
    SEND_ME_MY_ACTORS=8,
    SEND_OPENING_SCREEN=9,
    SEND_VERSION=10,
    HEARTBEAT=14,
    USE_OBJECT=16,
    HARVEST=21,
    SEND_PARTIAL_STATS=49,
    PING_RESPONSE=60,
    SET_ACTIVE_CHANNEL=61,
    LOG_IN=140,
    CREATE_CHAR=141,
    GET_DATE=230,
    GET_TIME=231,
    SERVER_STATS=232
};

void process_packet(int connection, unsigned char *packet); //, struct client_list_type *clients, struct guild_list_type *guilds, struct character_list_type *characters, struct map_list_type *maps, struct message_list_type *messages, struct channel_list_type *channels);

void send_server_text(int sock, int channel, char *text);

void send_get_active_channels(int connection);

int get_char_id(char *char_name);

int get_char_connection(char char_id);

void send_change_map(int connection, char *elm_filename);

/** RESULT  : make other actors in proximity visible to this actor

    RETURNS : void

    PURPOSE : ensures our actor can see other actors after log on or a map jump

    USAGE   : protocol.c process_packet
*/
void send_actors_to_client(int connection);

#endif // PROTOCOL_H_INCLUDED
