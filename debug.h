#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

void debug_raw_text(int connection, char *text, int text_len);

void debug_new_char_packet(unsigned char packet[1024]);

void debug_channels(int char_id);

void debug_char_struct(int connection);

void debug_map_client_list(int map_id);

void debug_channel_client_list(int chan);

#endif // DEBUG_H_INCLUDED
