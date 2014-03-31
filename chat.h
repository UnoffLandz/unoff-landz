#ifndef CHAT_H_INCLUDED
#define CHAT_H_INCLUDED

#define MAX_CHAN_SLOTS 3
#define MAX_CHANNELS 10

enum { // channel types
    CHAT_LOCAL,
    CHAT_PERSONAL,
    CHAT_GM,
    CHAT_SERVER,
    CHAT_MOD,
    CHAT_CHANNEL1,
    CHAT_CHANNEL2,
    CHAT_CHANNEL3,
    CHAT_MODPM,
    CHAT_SERVER_PM,
};

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

void list_clients_in_chan(int connection, int chan_number);

int get_chan_slot(int connection, int channel_number, int *slot);

int get_free_chan_slot(int char_id, int *slot);

int get_used_chan_slot(int char_id, int *slot);

int join_channel(int connection, int chan);

int leave_channel(int connection, int chan);

int process_chat(int connection, char *text_in);

int process_guild_chat(int connection, char *text_in);

int process_inter_guild_chat(int connection, char *guild_tag, char *message);

int get_guild_number(char *guild_tag, int *guild_id);

void add_client_to_channel(int connection, int chan);

void remove_client_from_channel(int connection, int chan);

void send_pm(int connection, char *receiver_name, char *message);


/** RESULT  : instructs client to set/change the active chat chan

    RETURNS : void

    PURPOSE : allows player to set/change the active chat chan

    USAGE   : chat.c join_channel, protocol.c process_packet
*/
void send_get_active_channels(int connection);

#endif // CHAT_H_INCLUDED
