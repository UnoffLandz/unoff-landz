#ifndef BROADCAST_H_INCLUDED
#define BROADCAST_H_INCLUDED

void broadcast_remove_actor_packet(int connection);

void broadcast_add_new_enhanced_actor_packet(int connection);

void broadcast_raw_text_packet(int connection, int channel, int chan_type, char *text);

void send_raw_text_packet(int sock, int chan_type, char *text);

void add_new_enhanced_actor_packet(int char_id, unsigned char *packet, int *packet_length);

void add_actor_packet(int char_id, int move, unsigned char *packet, int *packet_length);

void broadcast_actor_packet(int connection, unsigned char move, int destination_tile);

int get_proximity(int tile1, int tile2, int map_axis);

void broadcast_local_chat(int connection, char *text);

void broadcast_channel_chat(int channel, int sender_connection, char *text);

void broadcast_channel_event(int chan, int sender_connection, char *text_in);

void broadcast_guild_channel_chat(int guild_id, char *text);

#endif // BROADCAST_H_INCLUDED
