#ifndef CHAT_H_INCLUDED
#define CHAT_H_INCLUDED

#define MAX_CHANNELS 10
#define MAX_CHAN_SLOTS 4

#define LOCAL_CHAT_RANGE 10

struct channel_node_type{

    enum {CHAN_VACANT, CHAN_SYSTEM, CHAN_PERMANENT, CHAN_GUILD, CHAN_CHAT} chan_type;
    char channel_name[80];
    int owner_id; // could be char or guild depending on chan_type
    char password[80];
    char description[80];
};
struct channel_node_type channel[MAX_CHANNELS];

enum { //return values from process_guild_chat
    GM_INVALID,
    GM_NO_GUILD,
    GM_NO_PERMISSION,
    GM_SENT
};


enum { //return values for join_channel
    CHANNEL_JOINED,
    CHANNEL_NOT_JOINED
};

enum { //return values for leave channel
    CHANNEL_LEFT,
    CHANNEL_NOT_LEFT
};

enum { // return values for process_chat function
    CHAR_NOT_IN_CHAN=-1,
    CHAN_CHAT_SENT=0
};

/** RESULT  : determines if a player has a chan open

    RETURNS : NOT_FOUND(-1) if not in chan or chan slot number

    PURPOSE : used by list_clients_in_chan, leave_channel, broadcast_channel_chat

    NOTES   :
**/
int is_player_in_chan(int connection, int chan);


/** RESULT  : joins a chat_channel

    RETURNS : CHANNEL_JOINED/CHANNEL_NOT_JOINED

    PURPOSE :

    NOTES   :
**/
int join_channel(int connection, int chan);


/** RESULT  : leaves a chat_channel

    RETURNS : CHANNEL_LEFT/CHANNEL_NOT_LEFT

    PURPOSE :

    NOTES   :
**/
int leave_channel(int connection, int chan);


/** RESULT  : send a private message

    RETURNS : void

    PURPOSE :

    NOTES   :
**/
void send_pm(int connection, char *receiver_name, char *message);


/** RESULT  : lists participants in a chat channel

    RETURNS : void

    PURPOSE : sends a list of channels to channel participants when player joins a channel or
              sends the #CP command

    NOTES   :
**/
void list_characters_in_chan(int connection, int chan);




#endif // CHAT_H_INCLUDED
