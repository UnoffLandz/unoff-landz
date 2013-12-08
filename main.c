#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h> //required for timer
#include <time.h>

#include "global.h"
#include "initialisation.h"
#include "hash_commands.h"
#include "string_functions.h"
#include "protocol.h"
#include "broadcast.h"
#include "files.h"
#include "character_movement.h"
#include "chat.h"
#include "maps.h"

int find_free_connection_slot(){

    int i=0;

    for(i=0; i<clients.max; i++){

        if(clients.client[i]->status==LOGGED_OUT) return i;
    }

    return NO_FREE_SLOTS;
}


void close_connection_slot(int connection){

    int j=0;
    int char_id=clients.client[connection]->character_id;
    int guild_id=characters.character[char_id]->guild_id;
    int chan_colour=guilds.guild[guild_id]->log_off_notification_colour;
    int chan=0;
    char text_out[1024]="";

    if(clients.client[connection]->status==LOGGED_IN){

        //broadcast to local
        broadcast_remove_actor_packet(connection);

        //remove from map
        remove_client_from_map_list(connection, characters.character[char_id]->map_id);

        //remove from channels
        for(j=0; j<3; j++){

            chan=characters.character[char_id]->chan[j];

            if(chan>0) remove_client_from_channel(connection, characters.character[char_id]->chan[j]);
        }

        //update last in game time for char
        characters.character[char_id]->last_in_game=time(NULL);

        if(guild_id>0) {

            // broadcast to guild when char logs out
            sprintf(text_out, "%c%s LEFT THE GAME", chan_colour, characters.character[char_id]->char_name);
            broadcast_guild_channel_chat(guild_id, text_out);
        }

    }

    printf("Closed Sock %i\n", clients.client[connection]->sock);
    close(clients.client[connection]->sock);
    clients.client[connection]->status=LOGGED_OUT;
    clients.count--;
}

int main (void) {

    const uint16_t PORT=5555;

    int master_sock;
    int new_sock;
    int i=0, j=0;
    size_t clilen;
    struct sockaddr_in cli_addr, servername;
    unsigned char recv_buffer[1024];
    int bytes_recv;
    unsigned char packet[1024];
    int packet_length=0;
    int lsb, msb;
    char msg[1024]="";
    char text_out[1024]="";
    int yes=1;
    time_t current_utime;

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

    //lose port messages
    if(setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1){
        perror ("set socket option error");
        exit(EXIT_FAILURE);
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

            printf("New connection from %s on socket %d\n", inet_ntoa(cli_addr.sin_addr), new_sock);
            bzero(recv_buffer, 1024);

            // find free connection slot
            i=find_free_connection_slot();

            if(i!=NO_FREE_SLOTS){

                //free slot found
                clients.client[i]->sock=new_sock;
                clients.client[i]->status=CONNECTED;
                clients.client[i]->packet_buffer_length=0;
                strcpy(clients.client[j]->ip_address, inet_ntoa(cli_addr.sin_addr));

                read_motd(new_sock);

                clients.count++;
            }
            else{
                // no free slots found
                log_event(EVENT_ERROR, "server max clients exceeded in function main");
                sprintf(msg, "%cSorry, but all available connections to the game are being used. Please try later.", c_red2+127);
                send_server_text(new_sock, CHAT_SERVER, msg);
                close(new_sock);
            }
        }

        if(clients.count>0){

            for(i=0; i<clients.max; i++){

                if(clients.client[i]->status==CONNECTED || clients.client[i]->status==LOGGED_IN){

                    // get time
                    gettimeofday(&time_check, NULL);
                    current_utime=time_check.tv_usec;

                    // check if sock is lagged
                    if(clients.client[i]->status==LOGGED_IN && clients.client[i]->time_of_last_heartbeat+26<time_check.tv_sec){

                        close_connection_slot(i);
                        printf("Closed Sock (client lagged out) %i\n", clients.client[i]->sock);

                        sprintf(text_out, "client [%i]  char [%s]lagged out\n", i, characters.character[clients.client[i]->character_id]->char_name);
                        log_event(EVENT_SESSION, text_out);

                        break;
                    }

                    // check sock status
                    bytes_recv=recv(clients.client[i]->sock, recv_buffer, 1024 ,0);

                    // check if sock is dead
                    if(bytes_recv==0){

                        close_connection_slot(i);

                        printf("Closed Sock (client logged-off) %i\n", clients.client[i]->sock);

                        sprintf(text_out, "client [%i]  char [%s]logged-off\n", i, characters.character[clients.client[i]->character_id]->char_name);
                        log_event(EVENT_SESSION, text_out);

                        break;
                    }

                    // check if sock has new data and add to buffer
                    if(bytes_recv>0) {

                        printf("Bytes received %i\n", bytes_recv);

                        for(j=0; j<bytes_recv; j++){
                            clients.client[i]->packet_buffer[clients.client[i]->packet_buffer_length+j]=recv_buffer[j];
                        }
 /*
                        memmove(clients.client[i]->buffer + clients.client[i]->buffer_length, recv_buffer, bytes_recv);
 */
                        clients.client[i]->packet_buffer_length=clients.client[i]->packet_buffer_length+bytes_recv;
                    }

                    //handle movement
                    process_char_move(i, current_utime);

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

                                //update heartbeat (we do this each time we receive any kind of message from the client)
                                clients.client[i]->time_of_last_heartbeat=time_check.tv_sec;

                                //process packet
                                process_packet(i, packet);

                                // remove packet from buffer
                                // memmove(clients.client[i]->buffer, clients.client[i]->buffer+packet_length, clients.client[i]->buffer_length-packet_length);
                                clients.client[i]->packet_buffer_length=clients.client[i]->packet_buffer_length-packet_length;

                                for(j=0;j <=clients.client[i]->packet_buffer_length; j++){
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

