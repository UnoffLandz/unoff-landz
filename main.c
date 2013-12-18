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
#include <ev.h>

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
#include "motd.h"

/* credit for the underlying libev socket code goes to Pierce <jqug123321@gmail.com>, details of which can be
found @ http://jqug.blogspot.co.uk/2013/02/libev-socket.html */

ev_timer timeout_watcher;

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

    clients.client[connection]->status=LOGGED_OUT;
    clients.count--;
}

void recv_data(struct ev_loop *loop, struct ev_io *watcher, int revents){

    unsigned char buffer[1024];
    unsigned char packet[1024];
    char text_out[1024]="";

    int lsb=0, msb=0;
    int packet_length=0;
    int j=0;

    int read = recv(watcher->fd, buffer, sizeof(buffer), 0);

    //read error
    if (read < 0) {

        if(errno==104){

            //client has terminated prematurely (probably due to not having a required map file)
            printf("read error in function recv_data (probably due to bad map change) %i\n", watcher->fd);
            close_connection_slot(watcher->fd);

            sprintf(text_out, "client [%i]  char [%s] was terminated by the server in function recv_data\n", watcher->fd, characters.character[clients.client[watcher->fd]->character_id]->char_name);
            log_event(EVENT_ERROR, text_out);

            ev_io_stop(loop, watcher);
            free(watcher);

            return;
        }
        else {
            //unknown read error
            perror("read error in function recv_data");

            sprintf(text_out, "client [%i]  char [%s] read error [%i]in function recv_data", watcher->fd, characters.character[clients.client[watcher->fd]->character_id]->char_name, errno);
            log_event(EVENT_ERROR, text_out);
            exit(EXIT_FAILURE);

        }
    }

    //client disconnect
    else if (read == 0) {

        printf("Closed Sock (client logged off) %i\n", watcher->fd);
        close_connection_slot(watcher->fd);

        sprintf(text_out, "client [%i]  char [%s]logged off\n", watcher->fd, characters.character[clients.client[watcher->fd]->character_id]->char_name);
        log_event(EVENT_SESSION, text_out);

        ev_io_stop(loop, watcher);
        free(watcher);

        return;
    }

    //copy new bytes to client packet buffer(memcpy doesn't work)
    printf("bytes %i\n", read);

    for(j=0; j<read; j++){
        clients.client[watcher->fd]->packet_buffer[clients.client[watcher->fd]->packet_buffer_length]=buffer[j];
        clients.client[watcher->fd]->packet_buffer_length++;
    }

    // check if any data in buffer
    if(clients.client[watcher->fd]->packet_buffer_length>0) {

        //if we have data in the buffer then read all the available packets
        do {
            lsb=clients.client[watcher->fd]->packet_buffer[1];
            msb=clients.client[watcher->fd]->packet_buffer[2];

            packet_length=lsb+(msb*256)+2;

            //update heartbeat (we do this each time we receive any data from the client)
            clients.client[watcher->fd]->time_of_last_heartbeat=time_check.tv_sec;

            // if insufficient data received then wait for more data
            if(clients.client[watcher->fd]->packet_buffer_length<packet_length) break;

            // copy packet from buffer
            for(j=0; j<packet_length; j++){
                packet[j]=clients.client[watcher->fd]->packet_buffer[j];
            }

            //process packet
            process_packet(watcher->fd, packet);

            // remove packet from buffer
            clients.client[watcher->fd]->packet_buffer_length=clients.client[watcher->fd]->packet_buffer_length-packet_length;

            for(j=0; j<=clients.client[watcher->fd]->packet_buffer_length; j++){
                clients.client[watcher->fd]->packet_buffer[j]=clients.client[watcher->fd]->packet_buffer[j+packet_length];
            }

        } while(1);
    }
}

void accept_client(struct ev_loop *loop, struct ev_io *watcher, int revents){

    struct sockaddr_in client_address;
    size_t client_len = sizeof(client_address);
    int client_sockfd;
    struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

    client_sockfd = accept( watcher->fd, (struct sockaddr *)&client_address, &client_len);

    //accept client error
    if(client_sockfd < 0){
        perror("accept ");
        return;
    }

    //add watcher to connect client fd
    ev_io_init(w_client, recv_data, client_sockfd, EV_READ);
    ev_io_start(loop, w_client);

    printf("New connection from %s on socket %d\n", inet_ntoa(client_address.sin_addr), client_sockfd);

    clients.client[client_sockfd]->status=CONNECTED;
    clients.client[client_sockfd]->packet_buffer_length=0;
    strcpy(clients.client[client_sockfd]->ip_address, inet_ntoa(client_address.sin_addr));

    send_motd(client_sockfd);

    clients.count++;
}

static void timeout_cb(EV_P_ struct ev_timer* timer, int revents){

    int i=0;
    time_t current_utime;
    char text_out[1024]="";

    gettimeofday(&time_check, NULL);
    current_utime=time_check.tv_usec;

    for(i=0; i<clients.max; i++){

        process_char_move(i, current_utime);

        if(clients.client[i]->status==LOGGED_IN && clients.client[i]->time_of_last_heartbeat+26<time_check.tv_sec){

            printf("Closed Sock (client lagged out) %i %s\n", i, characters.character[clients.client[i]->character_id]->char_name);
            close_connection_slot(i);

            sprintf(text_out, "client [%i]  char [%s]lagged out\n", i, characters.character[clients.client[i]->character_id]->char_name);
            log_event(EVENT_SESSION, text_out);

            //ev_io_stop(loop, watcher);
            //free(watcher);
            close(i);
        }
    }

    // repeat
    ev_timer_again(loop, &timeout_watcher);
}

int main(void) {

    int server_sockfd;
    int server_len;
    struct sockaddr_in server_address;
    struct ev_loop *loop = ev_default_loop(0);
    struct ev_io stdin_watcher;
    int bReuseaddr = 1;

    //set server start time for motd
    server_start_time=time(NULL);

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

    //create server socket & bind it to socket address
    if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5555);
    server_len = sizeof(server_address);

    //no wait time to reuse the socket
    if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(bReuseaddr))==-1){
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if(bind(server_sockfd, (struct sockaddr *)&server_address, server_len)==-1){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //listen on server socket
    if(listen(server_sockfd, 5)==-1){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // initialize a timer watcher
    ev_timer_init(&timeout_watcher, timeout_cb, 0.05, 0.05);
    ev_timer_start(loop, &timeout_watcher);

    //create watcher to accept connection
    ev_io_init(&stdin_watcher, accept_client, server_sockfd, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    printf("server is ready\n");

    //start event loop
    while (1) {
        ev_loop(loop, 0);
    }

    return 0;
}
