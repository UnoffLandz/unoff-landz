#ifndef CLIENT_PROTOCOL_HANDLER_H_INCLUDED
#define CLIENT_PROTOCOL_HANDLER_H_INCLUDED

/** RESULT  : processes a data packet received from the client

    RETURNS : void

    PURPOSE : reduce need for code in main.c
*/
void process_packet(int connection, unsigned char *packet);
//void process_packet(int connection, unsigned char *packet, struct ev_loop *loop);

#endif // CLIENT_PROTOCOL_HANDLER_H_INCLUDED
