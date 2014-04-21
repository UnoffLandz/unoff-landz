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
#include "harvesting.h"
#include "database.h"

#define PORT 5555
#define HEARTBEAT_INTERVAL 26

/** credit for the underlying libev socket code goes to Pierce <jqug123321@gmail.com>, details of which can be
found @ http://jqug.blogspot.co.uk/2013/02/libev-socket.html **/

ev_timer timeout_watcher;

void close_connection_slot(int connection){

    //this function is called from the accept and timeout callbacks

    int j=0;
    int guild_id=clients.client[connection]->guild_id;
    int chan_colour=guilds.guild[guild_id]->log_off_notification_colour;
    int chan=0;
    char text_out[1024]="";

    if(clients.client[connection]->status==LOGGED_IN){

        //broadcast to local
        broadcast_remove_actor_packet(connection);

        //remove from map
        remove_client_from_map(connection, clients.client[connection]->map_id);

        //remove from channels
        for(j=0; j<3; j++){

            chan=clients.client[connection]->chan[j];

            if(chan>0) remove_client_from_channel(connection, clients.client[connection]->chan[j]);
        }

        //update last in game time for char
        clients.client[connection]->last_in_game=time(NULL);

        //update total time this char has been played
        clients.client[connection]->time_played+=(int) clients.client[connection]->last_in_game - (int) clients.client[connection]->session_commenced;

        //check if guild member
        if(guild_id>0) {

            // broadcast to guild when char logs out
            sprintf(text_out, "%c%s LEFT THE GAME", chan_colour, clients.client[connection]->char_name);
            broadcast_guild_channel_chat(guild_id, text_out);
        }

    }


    //set the connection slot to indicate its available for use
    clients.client[connection]->status=LOGGED_OUT;

    //update the database before we zero the struct
    update_db_char_time_played(connection);
    update_db_char_last_in_game(connection);

    clients.client[connection]->packet_buffer_length=0;
    clients.client[connection]->character_id=0;
    clients.client[connection]->path_max=0;
    clients.client[connection]->path_count=0;
    clients.client[connection]->time_of_last_move=0;
    clients.client[connection]->time_of_last_heartbeat=0;
    clients.client[connection]->time_of_last_harvest=0;
    clients.client[connection]->harvest_flag=0;
    clients.client[connection]->inventory_image_id=0;
    clients.client[connection]->inventory_slot=0;
}

void recv_data(struct ev_loop *loop, struct ev_io *watcher, int revents){

    (void)(revents);//removes unused parameter warning

    unsigned char buffer[1024];
    unsigned char packet[1024];

    int lsb=0, msb=0;
    int packet_length=0;
    int j=0;

    int read = recv(watcher->fd, buffer, sizeof(buffer), 0);

    //read error
    if (read < 0) {

        if(errno==104){

            //client has terminated prematurely (probably due to not having a required map file)
            close_connection_slot(watcher->fd);

            log_event2(EVENT_SESSION, "client [%i]  char [%s] was terminated by the server in function recv_data", watcher->fd, clients.client[watcher->fd]->char_name);

            ev_io_stop(loop, watcher);
            free(watcher);

            return;
        }
        else {

            //client has terminated prematurely (probably due to closing client whilst char was moving)
            log_event2(EVENT_SESSION, "client [%i]  char [%s] read error [%i]in function recv_data", watcher->fd, clients.client[watcher->fd]->char_name, errno);

            close_connection_slot(watcher->fd);

            ev_io_stop(loop, watcher);
            free(watcher);

            return;
        }
    }

    //client disconnect
    else if (read == 0) {

        log_event2(EVENT_SESSION, "client [%i] char [%s] disconnected\n", watcher->fd, clients.client[watcher->fd]->char_name);

        close_connection_slot(watcher->fd);

        ev_io_stop(loop, watcher);
        free(watcher);

        return;
    }

    //copy new bytes to client packet buffer(memcpy doesn't work)
    log_event2(EVENT_SESSION, "bytes %i", read);

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
            //process_packet(watcher->fd, packet);
            process_packet(watcher->fd, packet, loop);

            // remove packet from buffer
            clients.client[watcher->fd]->packet_buffer_length=clients.client[watcher->fd]->packet_buffer_length-packet_length;

            for(j=0; j<=clients.client[watcher->fd]->packet_buffer_length; j++){
                clients.client[watcher->fd]->packet_buffer[j]=clients.client[watcher->fd]->packet_buffer[j+packet_length];
            }

        } while(1);
    }
}

void accept_client(struct ev_loop *loop, struct ev_io *watcher, int revents){

    (void)(revents);//removes unused parameter warning

    struct sockaddr_in client_address;
    size_t client_len = sizeof(client_address);
    int client_sockfd;
    struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

    client_sockfd = accept( watcher->fd, (struct sockaddr *)&client_address, &client_len);

    //check that sock is within client array range
    if(client_sockfd>=MAX_CLIENTS) {

        log_event2(EVENT_ERROR, "new client socket [%i] outside client array max range [0 - %i]", client_sockfd, MAX_CLIENTS);
        exit(EXIT_FAILURE);
    }

    //accept client error
    if(client_sockfd < 0){

        log_event2(EVENT_ERROR, "client [%i] error in accept_client", client_sockfd);
        return;
    }

    log_event2(EVENT_SESSION, "Connection from address %s on socket %d", inet_ntoa(client_address.sin_addr), client_sockfd);

    //add watcher to connect client fd
    ev_io_init(w_client, recv_data, client_sockfd, EV_READ);
    ev_io_start(loop, w_client);

    //set up connection data entry in client struct
    clients.client[client_sockfd]->status=CONNECTED;
    clients.client[client_sockfd]->packet_buffer_length=0;
    strcpy(clients.client[client_sockfd]->ip_address, inet_ntoa(client_address.sin_addr));

    gettimeofday(&time_check, NULL);
    clients.client[client_sockfd]->time_of_last_heartbeat=time_check.tv_sec;

    //send welcome message and motd to client
    send_server_text(client_sockfd, CHAT_SERVER, SERVER_WELCOME_MSG);
    send_motd(client_sockfd);
    send_server_text(client_sockfd, CHAT_SERVER, "\nHit any key to continue...\n");
}

static void timeout_cb(EV_P_ struct ev_timer* timer, int revents){

    (void)(timer);//removes unused parameter warning
    (void)(revents);//removes unused parameter warning

    int i=0;
    time_t current_utime;
    //time_t current_time;

    gettimeofday(&time_check, NULL);
    current_utime=time_check.tv_usec;
    //current_time=time_check.tv_sec;

    //at each timeout check through each connect client and process pending actions
    for(i=0; i<clients.max; i++){

        if(clients.client[i]->status==LOGGED_IN || clients.client[i]->status==CONNECTED) {

            //check for lagged connection
            if(clients.client[i]->time_of_last_heartbeat+HEARTBEAT_INTERVAL< time_check.tv_sec){

                log_event2(EVENT_ERROR, "client [%i]  char [%s] lagged out", i, clients.client[i]->char_name);

                close_connection_slot(i);

                close(i);
            }

            //process timed actions
            if(clients.client[i]->status==LOGGED_IN) {

                process_char_move(i, current_utime);
                //process_harvesting(i, current_time);
            }
        }
    }

    // repeat
    ev_timer_again(loop, &timeout_watcher);
}

int main(void) {

    //load_e3d("house1.e3d");
    //exit(1);

    int server_sockfd;
    int server_len;
    struct sockaddr_in server_address;
    struct ev_loop *loop = ev_default_loop(0);
    struct ev_io stdin_watcher;
    int bReuseaddr = 1;

    //clear the logs
    initialise_logs();

    //set server start time
    server_start_time=time(NULL);

    //initialise database
    open_database(DATABASE_FILE_NAME);
    if(database_table_count()==0) create_new_database();

    //initialise data structs
    initialise_channel_list(MAX_CHANNELS);
    initialise_channels();

    initialise_map_list(MAX_MAPS);
    load_maps();

    initialise_guild_list(MAX_GUILDS);
    load_guilds();

    initialise_client_list(MAX_CLIENTS);

    //initialise bag_list struct
    initialise_bag_list();

    //load data into lookup structs
    initialise_movement_vectors();

    //load database tables into memory
    initialise_3d_objects();
    initialise_items();
    initialise_races();

    //set global data
    get_last_char_created(); //loads details of the last char created into the game_data struct

    //create server socket & bind it to socket address
    if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1){

        log_event2(EVENT_ERROR, "socket failed in function main: module main.c");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);
    server_len = sizeof(server_address);

    //no wait time to reuse the socket
    if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(bReuseaddr))==-1){

        log_event2(EVENT_ERROR, "setsockopt failed in function main: module main.c");
        exit(EXIT_FAILURE);
    }

    //bind the server socket to an address
    if(bind(server_sockfd, (struct sockaddr *)&server_address, server_len)==-1){

        log_event2(EVENT_ERROR, "bind failed in function main: module main.c");
        exit(EXIT_FAILURE);
    }

    //listen on server socket
    if(listen(server_sockfd, 5)==-1){

        log_event2(EVENT_ERROR, "listen failed in function main: module main.c");
        exit(EXIT_FAILURE);
    }

    // initialize a timer watcher
    ev_timer_init(&timeout_watcher, timeout_cb, 0.05, 0.05);
    ev_timer_start(loop, &timeout_watcher);

    //create watcher to accept connection
    ev_io_init(&stdin_watcher, accept_client, server_sockfd, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    log_event2(EVENT_INITIALISATION, "server is ready");

    //start event loop
    while (1) {
        ev_loop(loop, 0);
    }

    return 0;
}
