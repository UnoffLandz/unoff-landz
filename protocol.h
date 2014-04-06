#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

enum { // client to server protocol
    RAW_TEXT=0,
    MOVE_TO=1,
    SEND_PM=2,
    GET_PLAYER_INFO=5,
    SIT_DOWN=7,
    SEND_ME_MY_ACTORS=8,
    SEND_OPENING_SCREEN=9,
    SEND_VERSION=10,
    HEARTBEAT=14,
    USE_OBJECT=16,
    LOOK_AT_INVENTORY_ITEM=19,
    MOVE_INVENTORY_ITEM=20,
    HARVEST=21,
    DROP_ITEM=22,
    LOOK_AT_MAP_OBJECT=27,
    SEND_PARTIAL_STATS=49,
    PING_RESPONSE=60,
    SET_ACTIVE_CHANNEL=61,
    LOG_IN=140,
    CREATE_CHAR=141,
    GET_DATE=230,
    GET_TIME=231,
    SERVER_STATS=232
};

enum { // return values for find_free_connection_slot
    NO_FREE_SLOTS=-1,
    FREE_SLOTS=0
};

enum { // sit down command instruction
    CHAR_SIT_DOWN=0,
    CHAR_STAND_UP=1
};

/** RESULT  : processes a data packet received from the client

    RETURNS : void

    PURPOSE : reduce need for code in main.c

    USAGE   : main.c
*/
//void process_packet(int connection, unsigned char *packet);
void process_packet(int connection, unsigned char *packet, struct ev_loop *loop);


/** RESULT  : sends a text string to the client

    RETURNS : void

    PURPOSE : function definition sends to sock rather than connection so that we can reach clients when
              a connection has not been allocated. This happens when a client tries to connect after the
              maximum number of connections has been exceeded, in which case, we need to be able to send
              a message to that client telling it to wait until a connection becomes available.

    USAGE   : lots
*/
void send_server_text(int sock, int channel, char *text);

/** RESULT  : make other actors in proximity visible to this actor

    RETURNS : void

    PURPOSE : ensures our actor can see other actors after log on or a map jump

    USAGE   : protocol.c process_packet
*/
void send_actors_to_client(int connection);


/** RESULT  : updates char stats

    RETURNS : void

    PURPOSE : send updated stats to client

    USAGE   : harvesting.c process_harvesting
*/
void send_partial_stats(int connection, int attribute_type, int attribute_level);


/** RESULT  : sends char stats

    RETURNS : void

    PURPOSE : send all char stats at log in

    USAGE   :  protocol.c process_packet
*/
void send_here_your_stats(int connection);

#endif // PROTOCOL_H_INCLUDED
