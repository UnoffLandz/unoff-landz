#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

enum { /* client to server protocol */
    RAW_TEXT=0,
    MOVE_TO=1,
    SEND_PM=2,
    SEND_ME_MY_ACTORS=8,
    SEND_OPENING_SCREEN=9,
    SEND_VERSION=10,
    HEARTBEAT=14,
    USE_OBJECT=16,
    PING_RESPONSE=60,
    LOG_IN=140,
    CREATE_CHAR=141,
    GET_DATE=230,
    GET_TIME=231,
};

void process_packet(int connection, unsigned char *packet); //, struct client_list_type *clients, struct guild_list_type *guilds, struct character_list_type *characters, struct map_list_type *maps, struct message_list_type *messages, struct channel_list_type *channels);

void send_server_text(int sock, int channel, char *text);

void send_get_active_channels(int sock, int active_chan, int chan1, int chan2, int chan3);

#endif // PROTOCOL_H_INCLUDED
