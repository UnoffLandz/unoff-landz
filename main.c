#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <string.h>
//#include <signal.h> //required for timer
//#include <sys/time.h> //required for timer

#include "global.h"
#include "initialisation.h"
#include "hash_commands.h"
#include "string_functions.h"
#include "protocol.h"
#include "broadcast.h"
#include "files.h"
#include "character_movement.h"

/*
void timer_handler (int signum){

 static int count = 0;
 printf ("timer expired %d times\n", ++count);
}
*/

int main (void) {

    const uint16_t PORT=5555;

    int master_sock;
    int new_sock;
    int i,j;
    size_t clilen;
    struct sockaddr_in cli_addr, servername;
    unsigned char recv_buffer[1024];
    int bytes_recv;
    unsigned char packet[1024];
    int packet_length=0;
    int sock_status;
    int lsb, msb;
    char msg[1024]="";
    int char_id;
    char text_out[1024]="";
    int chan_colour=0;
    int guild_chan_number=0;

/*
    struct sigaction sa;
    struct itimerval timer;

    // Install timer_handler as the signal handler for SIGVTALRM.
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction (SIGVTALRM, &sa, NULL);
*/

    //initialise data structs
    initialise_channel_list(MAX_CHANNELS);
    load_all_channels(CHANNEL_LIST_FILE);

    initialise_map_list(MAX_MAPS);
    load_all_maps(MAP_LIST_FILE);

    initialise_guild_list(MAX_GUILDS);
    load_all_guilds(GUILD_LIST_FILE);

    initialise_character_list(MAX_CHARACTERS);
    load_all_characters(CHARACTER_LIST_FILE);

    initialise_client_list(MAX_CLIENTS);

    printf("\n");

    /* Create the socket. */
    if((master_sock = socket (PF_INET, SOCK_STREAM, 0))==-1) {
        perror ("failed to create socket");
        exit (EXIT_FAILURE);
    }
    else {
        printf("master socket created\n");
    }

    /* make socket non-blocking */
    if((fcntl(master_sock, F_SETFL, O_NONBLOCK))==-1) {
        perror("failed to set master socket to non-blocking");
        exit(EXIT_FAILURE);
    }
    else {
        printf("master socket set non-blocking\n");
    }

    /* Bind the socket to an address */
    servername.sin_family = AF_INET;
    servername.sin_port = htons (PORT);
    servername.sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind (master_sock, (struct sockaddr *) &servername, sizeof (servername)) ==-1) {
        perror ("failed to bind socket");
        exit (EXIT_FAILURE);
    }
    else {
        printf("master socket bound to address\n");
    }

    /* Make socket listen */
    if (listen (master_sock, 5) < 0) {
        perror ("listen");
        exit (EXIT_FAILURE);
    }
    else {
        printf("master socket listening\n");
    }

    printf("\n");

    clilen=sizeof(cli_addr);

    while(1) {

        /* check for new connections */
        if((new_sock=accept(master_sock,(struct sockaddr *) &cli_addr, &clilen))!=-1) {

            if((fcntl(new_sock, F_SETFL, O_NONBLOCK))==-1) {
                perror("fcntl() failed in function main");
                exit(EXIT_FAILURE);
            }

            //printf("Server: connect from host %s, port %hd.\n", inet_ntoa (cli_addr.sin_addr), ntohs (cli_addr.sin_port));
            printf("New connection\n");
            bzero(recv_buffer, 1024);

            // find free connection slot
            for(i=0; i<clients.max; i++){
                if(clients.client[i]->status==LOGGED_OUT) break;
            }

            if(i<clients.max) {

                // free connection slot found
                clients.client[i]->sock=new_sock;
                clients.client[i]->status=CONNECTED;
                clients.client[i]->packet_buffer_length=0;
                //clients.client[i]->ip_address=???

                sprintf(msg, "WELCOME TO THE UNOFF SERVER");
                send_server_text(new_sock, CHAT_SERVER, msg);
                sprintf(msg, "---------------------------" );
                send_server_text(new_sock, CHAT_SERVER, msg);

                read_motd(new_sock);

                clients.count++;
/*
                //start timer to periodically save char data (position etc)
                if(clients.count>0){

                    printf(" Start save character data timer\n");

                    timer.it_value.tv_sec = 5;
                    timer.it_value.tv_usec = 0;
                    timer.it_interval.tv_sec = 5;
                    timer.it_interval.tv_usec = 0;

                    setitimer (ITIMER_VIRTUAL, &timer, NULL);
                }
*/
            }
            else {
                // no free connection slots found
                log_event(EVENT_ERROR, "server max clients exceeded in function main");
                sprintf(msg, "%cSorry, but all available connections to the game are being used. Please try later.", c_red2+127);
                send_server_text(new_sock, CHAT_SERVER, msg);
                close(new_sock);
            }
        }

        if(clients.count>0){

            for(i=0; i<clients.max; i++){

                if(clients.client[i]->status==CONNECTED || clients.client[i]->status==LOGGED_IN){

                    char_id=clients.client[i]->character_id;

                    // check sock status
                    sock_status=recv(clients.client[i]->sock, recv_buffer, 1024 ,0);

                    // check if sock is dead
                    if(sock_status==0){

                        broadcast_remove_actor_packet(i);

                        printf("Closed Sock %i\n", clients.client[i]->sock);

                        if(characters.character[char_id]->guild_id>0) {

                            // broadcast to guild when char logs out
                            guild_chan_number=guilds.guild[characters.character[char_id]->guild_id]->guild_chan_number;
                            chan_colour=guilds.guild[characters.character[char_id]->guild_id]->log_off_notification_colour;
                            sprintf(text_out, "%c%s LEFT THE GAME", chan_colour, characters.character[char_id]->char_name);
                            broadcast_raw_text_packet(i, guild_chan_number, CHAT_GM, text_out);
                        }

                        close(clients.client[i]->sock);
                        clients.client[i]->status=LOGGED_OUT;
                        clients.count--;

/*
                        //stop timer to periodically save char data (position etc)
                        if(clients.count==0){

                            printf(" Stop save character data timer\n");

                            timer.it_value.tv_sec = 0;
                            timer.it_value.tv_usec = 0;
                            timer.it_interval.tv_sec = 0;
                            timer.it_interval.tv_usec = 0;

                            setitimer (ITIMER_VIRTUAL, &timer, NULL);
                        }
*/
                    }

                    // check if sock has new data and add to buffer
                    if(sock_status>0) {

                        printf("Bytes received %i\n", sock_status);
                        bytes_recv=sock_status;

                         for(j=0; j<bytes_recv; j++){
                            clients.client[i]->packet_buffer[clients.client[i]->packet_buffer_length+j]=recv_buffer[j];
                        }
 /*
                        memmove(clients.client[i]->buffer + clients.client[i]->buffer_length, recv_buffer, bytes_recv);
 */
                        clients.client[i]->packet_buffer_length=clients.client[i]->packet_buffer_length+bytes_recv;
                    }

                    //handle movement
                    process_char_move(i);


                    //handle other messages
                    if(clients.client[i]->cmd_buffer_end>0){

                        for(j=0; j<clients.client[i]->cmd_buffer_end; j++){
                            packet_length=clients.client[i]->cmd_buffer[j][1]+(clients.client[i]->cmd_buffer[j][2]*256)+2;
                            send(clients.client[i]->sock, clients.client[i]->cmd_buffer[j], packet_length, 0);
                        }

                         clients.client[i]->cmd_buffer_end=0;
                    }

                    do {
                        // check if any data in buffer
                        if(clients.client[i]->packet_buffer_length>0) {

                            lsb=clients.client[i]->packet_buffer[1];
                            msb=clients.client[i]->packet_buffer[2];
                            packet_length=lsb+(msb*256)+2;

                            // check if data is data in buffer is sufficient to make a packet
                            if(clients.client[i]->packet_buffer_length>=packet_length) {

                                // copy packet from buffer
                                for(j=0; j<packet_length; j++){
                                    packet[j]=clients.client[i]->packet_buffer[j];
                                }

                                process_packet(i, packet);

                                // remove packet from buffer

                                // memmove(clients.client[i]->buffer, clients.client[i]->buffer+packet_length, clients.client[i]->buffer_length-packet_length);

                                clients.client[i]->packet_buffer_length=clients.client[i]->packet_buffer_length-packet_length;

                                for(j=0;j<=clients.client[i]->packet_buffer_length; j++){
                                    clients.client[i]->packet_buffer[j]=clients.client[i]->packet_buffer[j+packet_length];
                                }

                            }

                        }

                    } while(clients.client[i]->packet_buffer_length>=packet_length);
                } // end of live sockets block
                sleep(0.1);

            } // end of socket loop

        }
    }

}

