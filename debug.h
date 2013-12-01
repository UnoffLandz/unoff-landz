#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

void debug_raw_text(int connection, char *text, int text_len);

void debug_new_char_packet(unsigned char packet[1024]);

void debug_channels(int char_id);

#endif // DEBUG_H_INCLUDED
