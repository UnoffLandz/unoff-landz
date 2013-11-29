#ifndef CHAT_H_INCLUDED
#define CHAT_H_INCLUDED

void list_clients_in_chan(int connection, int chan_number);

int get_chan_slot(int char_id, int channel_number);

int get_free_chan_slot(int char_id);

int get_used_chan_slot(int char_id);

int join_channel(int connection, int chan);

int leave_channel(int connection, int chan);

void debug_channels(int char_id);

int process_chat(int connection, char *text_in);

int process_guild_chat(int connection, char *text_in);

int process_inter_guild_chat(int connection, char *text_in);

int get_guild_number(char *guild_tag);

#endif // CHAT_H_INCLUDED