/******************************************************************************************************************
    Copyright 2014, 2015 UnoffLandz

    This file is part of unoff_server_4.

    unoff_server_4 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    unoff_server_4 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/
/******************************************************************************************************************

                                    COMPILER SETTINGS

To compile server, set the following compiler flags :

    -std=c99                - target c99 compliance
    -Wconversion            - 64bit compliance
    -wall                   - all common compiler warnings
    -wextra                 - extra compiler warnings
    -pendantic              - strict iso c/c++ warnings
    -std=C++11              - target c++ 11 compliance
    -Wunreachable-code      - warn on unreachable code
    -Wredundant-decis       - warn on duplicate declarations

                                    LINKING INFORMATION

To compile server, link with the following libraries :

    libev.a                 - libev event library
    libsqlite3.so           - sqlite database library

****************************************************************************************************/
/***************************************************************************************************

                                TO - DO

Done - Fixed char sit/stand
Done - drop bag and other commands stop harvesting
Done - harvesting and other commands stop movement
Done - Fixed #gm
Done - NPC move script implemented
Done - #boat (reminds when boat leaves)
Done - trigger table, action table and boat table stubs
Done - replaced hard coded values for max char name and max char password
Done - Fixed bug unknown protocol packets are not being logged in packet log
Done - separate module for ops #commands
Done - separate module for devs #commands
Done - #TRACE_PATH implemented
Done - changed add_map function to remove supplementary details
Done - added hash commands so as map supplementary details can be added in-game
Done - added separate function to extract 3d object list from elm file
Done - fixed bug in get_nearest_unoccupied_tile
Done - #jump now contains option to move to first walkable tile
Done - new harvestables (including pumpkin
Done - batch load maps on startup from map.lst file

TEST multiple guild application handling

load objects and e3d's from text file
reload map_objects (needed for when new e3d/object is added
add npc wear item action
add npc sit/stand action
change all sql to use parameters rather than inserts

bag_proximity (reveal and unreveal) use destroy and create in place of revised client code
need #command to withdraw application to join guild
need #letter system to inform ppl if guild application has been approved/rejected also if guild member leaves
transfer server welcome message to the database
#command to change guild chan join/leave notification colours
remove character_type_name field from CHARACTER_TYPE_TABLE
map object reserve respawn
#command to #letter all members of a guild
finish script loading
#IG guild channel functionality
OPS #command to #letter all chars
need #command to #letter all guild members (guild master only)
implement guild stats
Table to separately record all drops/pick ups in db
Table to separately record chars leaving and joining guilds
save guild applicant list to database
document idle_buffer2.h
convert attribute struct so as attribute type can be addressed programatically
identify cause of stall after login (likely to be loading of inventory from db)
identify cause of char bobbing
put inventory slots in a binary blob (may solve stall on log in)
improve error handling on upgrade_database function
refactor function current_database_version
create circular buffer for receiving packets
need #function to describe char and what it is wearing)
document new database/struct relationships
finish char_race_stats and char_gender_stats functions in db_char_tbl.c
banned chars go to ban map (jail). Dead chars got to dead map (ghost and graveyard)

***************************************************************************************************/

#include <stdio.h>      //supports printf function
#include <string.h>     //supports memset and strcpy functions
#include <errno.h>      //supports errno function
#include <arpa/inet.h>  //supports recv and accept function
#include <ev.h>         //supports ev event library
#include <fcntl.h>      //supports fcntl
#include <unistd.h>     //supports close function, ssize_t data type

#include "server_parameters.h"
#include "global.h"
#include "logging.h"
#include "server_messaging.h"
#include "clients.h"
#include "game_data.h"
#include "db/database_functions.h"
#include "client_protocol_handler.h"
#include "server_protocol_functions.h"
#include "db/db_attribute_tbl.h"
#include "db/db_character_race_tbl.h"
#include "db/db_character_tbl.h"
#include "db/db_character_type_tbl.h"
#include "db/db_chat_channel_tbl.h"
#include "db/db_game_data_tbl.h"
#include "db/db_gender_tbl.h"
#include "db/db_map_tbl.h"
#include "db/db_season_tbl.h"
#include "db/db_object_tbl.h"
#include "db/db_e3d_tbl.h"
#include "db/db_map_object_tbl.h"
#include "db/db_upgrade.h"
#include "db/db_guild_tbl.h"
#include "date_time_functions.h"
#include "movement.h"
#include "server_start_stop.h"
#include "attributes.h"
#include "chat.h"
#include "characters.h"
#include "idle_buffer2.h"
#include "file_functions.h"
#include "objects.h"
#include "harvesting.h"
#include "gender.h"
#include "character_type.h"
#include "colour.h"
#include "broadcast_chat.h"
#include "broadcast_movement.h"
#include "packet.h"
#include "bags.h"
#include "string_functions.h"
#include "maps.h"
#include "client_protocol.h"
#include "npc.h"
#include "boats.h"
#include "game_time.h"

struct ev_io *libevlist[MAX_ACTORS] = {NULL};


extern int current_database_version();


//declare prototypes
void socket_accept_callback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void socket_read_callback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void socket_write_callback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void timeout_cb(EV_P_ struct ev_timer* timer, int revents);
void game_time_cb(EV_P_ struct ev_timer* timer, int revents);
void idle_cb(EV_P_ struct ev_idle *watcher, int revents);


void start_server(){

    /** RESULT   : starts the server

        RETURNS  : void

        PURPOSE  : code modularisation

        NOTES    :
    **/

    struct ev_loop *loop = ev_default_loop(0);

    struct ev_io *socket_watcher = (struct ev_io*)malloc(sizeof(struct ev_io));
    struct ev_idle *idle_watcher = (struct ev_idle*)malloc(sizeof(struct ev_idle));
    struct ev_timer *timeout_watcher = (struct ev_timer*)malloc(sizeof(struct ev_timer));
    struct ev_timer *game_time_watcher = (struct ev_timer*)malloc(sizeof(struct ev_timer));

    struct sockaddr_in server_addr;

    //check database version
    int database_version = get_database_version();

    if(database_version != REQUIRED_DATABASE_VERSION) {

        printf("Database version [%i] not equal to [%i] - use -U option to upgrade your database\n", database_version, REQUIRED_DATABASE_VERSION);
        return;
    }

    //clear garbage from structs
    memset(&client_socket, 0, sizeof(client_socket));
    memset(&clients, 0, sizeof(clients));

    // TODO (themuntdregger#1#): clear other structs, ie maps, e3d etc

    //load data from database into memory
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_e3ds();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_objects();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_maps();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_map_objects();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_char_races();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_genders();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_char_types();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_attributes();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_channels();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_game_data();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_seasons();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    load_db_guilds();
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    /** Experimental NPC code **/

    clients.client[0].client_node_status=CLIENT_NODE_USED;
    clients.client[0].player_type=NPC;
    strcpy(clients.client[0].char_name, "NPC_1");
    clients.client[0].map_id=1;
    clients.client[0].map_tile=27225;
    clients.client[0].char_type=1;
    clients.client[0].skin_type=0;
    clients.client[0].hair_type=0;
    clients.client[0].shirt_type=0;
    clients.client[0].pants_type=0;
    clients.client[0].boots_type=0;
    clients.client[0].head_type=0;
    clients.client[0].shield_type=0;
    clients.client[0].weapon_type=0;
    clients.client[0].cape_type=0;
    clients.client[0].helmet_type=0;
    clients.client[0].frame=0;
    clients.client[0].portrait_id=1;

    npc_trigger[0].trigger_node_status=TRIGGER_NODE_USED;
    npc_trigger[0].actor_node=0; //npc actor node
    npc_trigger[0].trigger_type=TOUCHED;
    npc_trigger[0].action_node=0;
    //could add race as trigger criteria

    npc_trigger[1].trigger_node_status=TRIGGER_NODE_USED;
    npc_trigger[1].actor_node=0; //npc actor node
    npc_trigger[1].trigger_type=SELECT_OPTION;
    npc_trigger[1].action_node=1;
    //could add time of day, or race as trigger criteria

    npc_action[0].action_node_status=ACTION_NODE_USED;
    npc_action[0].actor_node=0; //npc actor node
    npc_action[0].action_type=GIVE_BOAT_SCHEDULE;

    npc_action[1].action_node_status=ACTION_NODE_USED;
    npc_action[1].actor_node=0; //npc actor node
    npc_action[1].action_type=SELL_BOAT_TICKET;
    npc_action[1].boat_node=0;

    boat[0].departure_map_id=1;
    boat[0].departure_map_tile=4054;
    boat[0].destination_map_id=16;
    boat[0].destination_map_tile=77969;
    strcpy(boat[0].departure_message, "Welcome aboard Salty Sealines. Please stow your luggage in the hold and enjoy your voyage");
    boat[0].travel_time=2;
    boat[0].boat_payment_object_id=3; //gold coin
    boat[0].boat_payment_price=5;
    strcpy(boat[0].arrival_message, "thank you for sailing with Salty Sealines. We hope you enjoyed your voyage and will sail with us again");
    boat[0].boat_map_id=3;
    boat[0].boat_map_tile=4651;

    clients.client[1].client_node_status=CLIENT_NODE_USED;
    clients.client[1].player_type=NPC;
    strcpy(clients.client[1].char_name, "NPC_2");
    clients.client[1].map_id=1;
    clients.client[1].map_tile=27227;
    clients.client[1].char_type=1;
    clients.client[1].skin_type=0;
    clients.client[1].hair_type=0;
    clients.client[1].shirt_type=0;
    clients.client[1].pants_type=0;
    clients.client[1].boots_type=0;
    clients.client[1].head_type=0;
    clients.client[1].shield_type=0;
    clients.client[1].weapon_type=0;
    clients.client[1].cape_type=0;
    clients.client[1].helmet_type=0;
    clients.client[1].frame=0;
    clients.client[1].portrait_id=2;

    npc_trigger[2].trigger_node_status=TRIGGER_NODE_USED;
    npc_trigger[2].actor_node=1; //npc actor node
    npc_trigger[2].trigger_type=TOUCHED;
    npc_trigger[2].action_node=2;

    npc_trigger[3].trigger_node_status=TRIGGER_NODE_USED;
    npc_trigger[3].actor_node=1; //npc actor node
    npc_trigger[3].trigger_type=SELECT_OPTION;
    npc_trigger[3].action_node=3;

    npc_action[2].action_node_status=ACTION_NODE_USED;
    npc_action[2].actor_node=1;
    npc_action[2].action_type=GIVE_SALE_OPTIONS;
    npc_action[2].object_id_required=408; //carrots
    npc_action[2].object_amount_required=5;
    npc_action[2].object_id_given=3; //gold
    npc_action[2].object_amount_given=1;

    npc_action[3].action_node_status=ACTION_NODE_USED;
    npc_action[3].actor_node=1;
    npc_action[3].action_type=SELL_OBJECT;

    npc_trigger[4].trigger_node_status=TRIGGER_NODE_USED;
    npc_trigger[4].actor_node=0;
    npc_trigger[4].trigger_type=TIME;
    npc_trigger[4].trigger_time=180;
    npc_trigger[4].action_node=4;

    npc_action[4].action_node_status=ACTION_NODE_USED;
    npc_action[4].actor_node=0;
    npc_action[4].action_type=NPC_MOVE;
    npc_action[4].destination=4634;

    npc_trigger[5].trigger_node_status=TRIGGER_NODE_USED;
    npc_trigger[5].actor_node=0;
    npc_trigger[5].trigger_type=TIME;
    npc_trigger[5].trigger_time=0;
    npc_trigger[5].action_node=5;

    npc_action[5].action_node_status=ACTION_NODE_USED;
    npc_action[5].actor_node=0;
    npc_action[5].action_type=NPC_MOVE;
    npc_action[5].destination=27225;

    /***************************/

    //gather initial stats
    get_db_last_char_created(); //loads details of the last char created from the database into the game_data struct
    game_data.char_count=get_db_char_count();

    //create the master socket
    int sd;
    if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1){

        int errnum=errno;

        log_event(EVENT_ERROR, "failed to create master socket in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    //clear master socket struct and fill with data
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    log_event(EVENT_INITIALISATION, "master socket established on address [%s]: port [%i]", inet_ntoa(server_addr.sin_addr), PORT);

    //allow the master socket to be immediately reused following a server stop
    int bReuseaddr = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char*) &bReuseaddr, sizeof(bReuseaddr)) != 0) {

        int errnum=errno;

        log_event(EVENT_ERROR, "setsockopt failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    log_event(EVENT_INITIALISATION, "master socket set for reuse");

    //bind the master server socket to an address
    if(bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))==-1){

        int errnum=errno;

        log_event(EVENT_ERROR, "bind failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    log_event(EVENT_INITIALISATION, "master socket bound to address");

    //listen for incoming client connections on master socket
    if(listen(sd, 5)==-1){// TODO (themuntdregger#1#): convert hardcoded max for incoming clients so as this is specified in the database

        int errnum=errno;

        log_event(EVENT_ERROR, "listen failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    log_event(EVENT_INITIALISATION, "master socket listening for incoming connections");
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    //start the event watchers
    ev_timer_init(timeout_watcher, timeout_cb, 0.05, 0.05);// TODO (themuntdregger#1#): convert hard coded timeout for watcher to database item
    ev_timer_start(loop, timeout_watcher);

    log_event(EVENT_INITIALISATION, "timeout watcher started");

    ev_timer_init(game_time_watcher, game_time_cb, GAME_MINUTE_INTERVAL, GAME_MINUTE_INTERVAL);
    ev_timer_start(loop, game_time_watcher);

    log_event(EVENT_INITIALISATION, "game time watcher started");

    ev_io_init(socket_watcher, socket_accept_callback, sd, EV_READ);
    ev_io_start(loop, socket_watcher);

    log_event(EVENT_INITIALISATION, "socket watcher started");

    ev_idle_init(idle_watcher, idle_cb);
    ev_idle_start(loop, idle_watcher);

    log_event(EVENT_INITIALISATION, "idle watcher started");
    log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

    log_event(EVENT_INITIALISATION, "main loop started");

    while(1) {

        ev_run(loop, 0);
    }
}

void socket_accept_callback(struct ev_loop *loop, struct ev_io *watcher, int revents) {

    /** RESULT   : handles socket accept event

        RETURNS  : void

        PURPOSE  : handles new client connections

        NOTES    :
    **/

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sd;

    struct ev_io *client_watcher = (struct ev_io*) malloc(sizeof(struct ev_io));

    if (client_watcher == NULL) {

        log_event(EVENT_ERROR, "malloc failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    //catch errors in libev
    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    //accept connection on master socket
    client_sd = accept(watcher->fd, (struct sockaddr*) &client_addr, &client_len);

    if (client_sd ==-1) {

        int errnum=errno;

        log_event(EVENT_ERROR, "accept failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "socket [%i] error [%i] [%s]", watcher->fd, errnum, strerror(errnum));
        stop_server();
    }

    //get next free node on the actor array
    int actor_node=get_next_free_actor_node();

    //catch actor bounds
    if (actor_node==-1) {

        //catch connection bounds exceeded
        log_event(EVENT_ERROR, "new connection exceeds actor node max [%i]", MAX_ACTORS);

        send_text(client_sd, CHAT_SERVER, "\nSorry but the server is currently full\n");
        close(client_sd);
        return;
    }

    //catch socket bounds
    if (client_sd>MAX_SOCKETS) {

        //catch connection bounds exceeded
        log_event(EVENT_ERROR, "new connection [%i] exceeds socket node max [%i]", client_sd, MAX_SOCKETS);

        send_text(client_sd, CHAT_SERVER, "\nSorry but the server is currently full\n");
        close(client_sd);
        return;
    }

    //set up listener for the new client
    ev_io_init(client_watcher, socket_read_callback, client_sd, EV_READ);
    ev_io_start(loop, client_watcher);
    libevlist[client_sd] = client_watcher;

    //set up entry in client socket array
    client_socket[client_sd].actor_node=actor_node;
    client_socket[client_sd].socket_node_status=CLIENT_CONNECTED;

    //get client ip address
    strcpy(client_socket[client_sd].ip_address, inet_ntoa(client_addr.sin_addr));
    log_event(EVENT_SESSION, "client connection from ip address [%s]", client_socket[client_sd].ip_address);

    //start heartbeat
    gettimeofday(&time_check, NULL);
    client_socket[client_sd].time_of_last_heartbeat=time_check.tv_sec;

    //set up entry in actor array
    clients.client[actor_node].socket=client_sd;
    clients.client[actor_node].client_node_status=CLIENT_NODE_USED;
    clients.client[actor_node].player_type=PLAYER;

    //send welcome message and motd to client
    send_text(client_sd, CHAT_SERVER, SERVER_WELCOME_MSG);
    send_motd(client_sd);
    send_text(client_sd, CHAT_SERVER, "\nHit any key to continue...\n");
}


void socket_read_callback(struct ev_loop *loop, struct ev_io *watcher, int revents) {

    /** RESULT   : handles socket read event

        RETURNS  : void

        PURPOSE  : handles existing client connections

        NOTES    :
    **/

    unsigned char buffer[MAX_PACKET_SIZE]={0};

    //catch libev errors
    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    //catch libev read events
    if(EV_READ & revents){

        //get the actor node
        int actor_node=client_socket[watcher->fd].actor_node;

        //check the socket has been registered in the socket array
        if(client_socket[watcher->fd].socket_node_status==SOCKET_UNUSED){

            log_event(EVENT_ERROR, "data received from unregistered socket [%i] in function %s: module %s: line %i", watcher->fd, __func__, __FILE__, __LINE__);
            return;
        }

        //read the receive buffer
        ssize_t read=recv(watcher->fd, buffer, 512, 0);

        if (read ==-1) {

            int errnum=errno;

            switch(errno){

                case EINTR: {// non serious error so keep client connection

                    log_event(EVENT_SESSION, "read EINTR in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                    log_text(EVENT_SESSION, "sock [%i] error [%i] [%s]... ignoring", watcher->fd, errnum, strerror(errnum));
                    break;
                }

                case EWOULDBLOCK: {// non serious error so keep client connection

                    log_event(EVENT_SESSION, "read EWOULDBLOCK in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                    log_text(EVENT_SESSION, "sock [%i] error [%i] [%s]... ignoring", watcher->fd, errnum, strerror(errnum));
                    break;
                }

                default:{// serious error so kill client connection

                    log_event(EVENT_ERROR, "read error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                    log_text(EVENT_ERROR, "sock [%i] error [%i] [%s]... closing", watcher->fd, errnum, strerror(errnum));

                    //close socket and stop watcher
                    close_connection_slot(actor_node);
                    ev_io_stop(loop, libevlist[watcher->fd]);
                    free(libevlist[watcher->fd]);
                    libevlist[watcher->fd] = NULL;

                    //clear the structs
                    memset(&client_socket, 0, sizeof(client_socket[watcher->fd]));
                    memset(&clients.client[actor_node], 0, sizeof(clients.client[actor_node]));
                }
            }

            return;
        }

        //client closed
        if (read == 0) {

            if (libevlist[watcher->fd]!= NULL) {

                log_event(EVENT_SESSION, "client [%i] logged-off on socket [%i]", actor_node, watcher->fd);

                //close socket and stop watcher
                close_connection_slot(actor_node);
                ev_io_stop(loop, libevlist[watcher->fd]);
                free(libevlist[watcher->fd]);
                libevlist[watcher->fd] = NULL;

                //clear the structs
                memset(&client_socket, 0, sizeof(client_socket[watcher->fd]));
                memset(&clients.client[actor_node], 0, sizeof(clients.client[actor_node]));
            }

            return;
        }

        //data received from client
        if(read>0){

            char char_name[MAX_CHAR_NAME_LEN]="";
            int actor_node=client_socket[watcher->fd].actor_node;

            if(client_socket[watcher->fd].socket_node_status==CLIENT_LOGGED_IN){

                strcpy(char_name, clients.client[actor_node].char_name);
            }
            else {

                sprintf(char_name, "not logged in");
            }

            log_event(EVENT_PACKET, "bytes [%i] received from socket [%i] for char [%s]", read, watcher->fd, char_name);

            //copy new bytes to client packet buffer
            memcpy(client_socket[watcher->fd].packet_buffer + client_socket[watcher->fd].packet_buffer_length, &buffer, (size_t)read);
            client_socket[watcher->fd].packet_buffer_length += (size_t)read;

            //if data is in the buffer then process it
            if(client_socket[watcher->fd].packet_buffer_length>0) {

                do {

                    //update client heartbeat any time data is received
                    client_socket[watcher->fd].time_of_last_heartbeat=time_check.tv_sec;

                    //if insufficient data to complete a packet then break from loop and wait for more data
                    size_t packet_length=get_packet_length(client_socket[watcher->fd].packet_buffer);
                    if(client_socket[watcher->fd].packet_buffer_length < packet_length) break;

                    //process packet
                    process_packet(actor_node, client_socket[watcher->fd].packet_buffer);

                    //remove packet from buffer and process
                    client_socket[watcher->fd].packet_buffer_length-=packet_length;
                    memmove(client_socket[watcher->fd].packet_buffer, client_socket[watcher->fd].packet_buffer + packet_length, client_socket[watcher->fd].packet_buffer_length);

                //continue to process buffer as long as there's data
                } while(client_socket[watcher->fd].packet_buffer_length>0);
            }
        }
    }
}


void game_time_cb(EV_P_ struct ev_timer* timer, int revents){

    /**     RESULT   : handles timeout event

            RETURNS  : void

            PURPOSE  : updates the game time

            NOTES    :
    **/

    (void)(timer);//removes unused parameter warning
    (void)(loop);

    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    update_game_time();
 }


void timeout_cb(EV_P_ struct ev_timer* timer, int revents){

    /** RESULT   : handles timeout event

        RETURNS  : void

        PURPOSE  : handles fixed interval processing tasks

        NOTES    :
    **/

    (void)(timer);//removes unused parameter warning
    (void)(loop);

    //catch evlib error
    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    //update time_check struct
    gettimeofday(&time_check, NULL);

    //check through all actors and process pending actions
    for(int i=0; i<MAX_ACTORS; i++){

        //process player related factors
        if(clients.client[i].player_type==PLAYER){

            int socket=clients.client[i].socket;

            //check for lagged connection
            if(client_socket[socket].socket_node_status!=SOCKET_UNUSED){

                if(client_socket[socket].time_of_last_heartbeat + HEARTBEAT_INTERVAL < time_check.tv_sec){

                    log_event(EVENT_SESSION, "client [%i] char [%s] lagged out", i, clients.client[i].char_name);

                    //broadcast that char was eaten by the Grue
                    char text_out[160]="";
                    sprintf(text_out, "Poor old %s was eaten by the Grue", clients.client[i].char_name);
                    broadcast_local_chat(i, text_out);

                    //close connection and stop watcher
                    close_connection_slot(i);
                    ev_io_stop(loop, libevlist[socket]);
                    free(libevlist[socket]);
                    libevlist[socket] = NULL;

                    //clear structs
                    memset(&clients.client[i], 0, sizeof(clients.client[i]));
                    memset(&client_socket[socket], 0, sizeof(client_socket[socket]));
                }
            }

            //check for kill flag on connection
            if(client_socket[socket].socket_node_status!=SOCKET_UNUSED){

                if(client_socket[socket].kill_connection==true){

                    //close socket and stop watcher
                    close_connection_slot(i);
                    ev_io_stop(loop, libevlist[socket]);
                    free(libevlist[socket]);
                    libevlist[socket] = NULL;

                    //clear structs
                    memset(&clients.client[i], 0, sizeof(clients.client[i]));
                    memset(&client_socket[socket], 0, sizeof(client_socket[socket]));
                }
            }

            //update game time for connection
            if(client_socket[socket].socket_node_status!=SOCKET_UNUSED){

                if(clients.client[i].time_of_last_minute+GAME_MINUTE_INTERVAL<time_check.tv_sec){

                    clients.client[i].time_of_last_minute=time_check.tv_sec;
                    send_new_minute(socket, game_data.game_minutes);

                    //update database with time char was last in game
                    push_sql_command("UPDATE CHARACTER_TABLE SET LAST_IN_GAME=%i WHERE CHAR_ID=%i;",(int)clients.client[i].time_of_last_minute, clients.client[i].character_id);
                }
            }

            //check for boat departures
            if(client_socket[socket].socket_node_status!=SOCKET_UNUSED){

                //check if char is book on boat
                if(clients.client[i].boat_booked==true){

                    //check if it's time for boat to depart
                    if(game_data.game_minutes>=clients.client[i].boat_departure_time){

                        int boat_node=clients.client[i].boat_node;

                        //set boat destination arrival time
                        clients.client[i].boat_arrival_time=clients.client[i].boat_departure_time + boat[boat_node].travel_time;

                        //check if char is close enough to boat
                        int departure_map_id=boat[boat_node].departure_map_id;
                        int departure_map_tile=boat[boat_node].departure_map_tile;
                        int boat_proximity=get_proximity(clients.client[i].map_tile, departure_map_tile, departure_map_id);

                        //jump char to boat map
                        if(boat_proximity<BOAT_CATCH_PROXIMITY && departure_map_id==clients.client[i].map_id){

                            if(move_char_between_maps(i, boat[boat_node].boat_map_id, boat[boat_node].boat_map_tile)==false){

                                log_event(EVENT_ERROR, "invalid map [%i] in function %s: module %s: line %i", boat[boat_node].boat_map_id, __func__, __FILE__, __LINE__);
                                stop_server();
                            }


                            send_text(clients.client[i].socket, CHAT_SERVER, "%s", boat[boat_node].departure_message);
                        }
                        else {

                            send_text(clients.client[i].socket, CHAN_CHAT, "%cOops, you missed the boat", 127+c_red3);
                        }

                        clients.client[i].boat_booked=false;
                        clients.client[i].on_boat=true;
                    }
                }
            }

            //check for boat arrivals
            if(client_socket[socket].socket_node_status!=SOCKET_UNUSED){

                //check if char is on a boat
                if(clients.client[i].on_boat==true){

                    //if boat has reached destination jump char to new map
                    if(game_data.game_minutes >= clients.client[i].boat_arrival_time){

                        int boat_node=clients.client[i].boat_node;

                        if(move_char_between_maps(i, boat[boat_node].destination_map_id, boat[boat_node].destination_map_tile)==false){

                            log_event(EVENT_ERROR, "invalid map [%i] in function %s: module %s: line %i", boat[boat_node].destination_map_id, __func__, __FILE__, __LINE__);
                            stop_server();
                        }

                        send_text(clients.client[i].socket, CHAT_SERVER, "%s", boat[boat_node].arrival_message);

                        clients.client[i].on_boat=false;
                    }
                }
            }
        }

        //process all actor related factors (NPC and Players)
        if(clients.client[i].client_node_status==CLIENT_NODE_USED) {

            //process char movements for actors
            process_char_move(i, time_check.tv_usec); //use milliseconds

            //process char harvesting for actors
            process_char_harvest(i, time_check.tv_sec); //use seconds
        }

        //process npc timed triggers
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==NPC) {

            for(int j=0; j<MAX_NPC_TRIGGERS; j++){

                if(npc_trigger[j].trigger_node_status==TRIGGER_NODE_UNUSED) break;

                if(npc_trigger[j].actor_node==i
                && npc_trigger[j].trigger_type==TIME
                && npc_trigger[j].trigger_time==game_data.game_minutes){

                    int action_node=npc_trigger[j].action_node;
                    int npc_destination=npc_action[action_node].destination;

                    //only start npc move is npc is standing still and is not already
                    //at destination
                    if(npc_action[action_node].action_type==NPC_MOVE
                    && clients.client[i].path_count==0
                    && clients.client[i].map_tile!=npc_destination){

                        start_char_move(i, npc_destination);
                    }
                }
            }
        }
    }

    // check bags for poof time
    for(int i=0; i<MAX_BAGS; i++){

         if(bag[i].bag_refreshed>0 && bag[i].bag_refreshed + BAG_POOF_INTERVAL < time_check.tv_sec){

            //poof the bag
            broadcast_destroy_bag_packet(i);
        }
    }
}


void idle_cb (struct ev_loop *loop, struct ev_idle *watcher, int revents){

    /** RESULT   : handles server idle event

        RETURNS  : void

        PURPOSE  : enables idle event to be used for low priority processing tasks

        NOTES    :
    **/

    (void)(loop);
    (void)(watcher);

    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    process_idle_buffer2();
}


int main(int argc, char *argv[]){

    /** RESULT   : handles command line arguments

        RETURNS  : dummy

        PURPOSE  : allows program to be started in different modes

        NOTES    :
    **/

    printf("Server - version %s.%s %s\n\n", VERSION, BUILD, BUILD_DATE);

    char db_filename[80]=DEFAULT_DATABASE_FILE_NAME;

    struct{

        bool start_server;
        bool create_database;
        bool upgrade_database;
        bool load_map;
        bool list_maps;
        bool help;
    }option;

    //clear struct to prevent garbage
    memset(&option, 0, sizeof(option));

    //set server start time
    game_data.server_start_time=time(NULL);

    //prepare start time for console and log message
    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    get_time_stamp_str(game_data.server_start_time, time_stamp_str);
    get_verbose_date_str(game_data.server_start_time, verbose_date_stamp_str);

    //clear logs
    initialise_logs();

    //parse command line
    log_text(EVENT_INITIALISATION, "Command line option count [%i]", argc);
    log_text(EVENT_INITIALISATION, "Parse command line...");

    for(int i=0; i<argc; i++){

        if (strcmp(argv[i], "-S") == 0)option.start_server=true;
        if (strcmp(argv[i], "-C") == 0)option.create_database=true;
        if (strcmp(argv[i], "-U") == 0)option.upgrade_database=true;
        if (strcmp(argv[i], "-M") == 0)option.load_map=true;
        if (strcmp(argv[i], "-L") == 0)option.list_maps=true;
        if (strcmp(argv[i], "-H") == 0)option.help=true;

        log_text(EVENT_INITIALISATION, "%i [%s]", i, argv[i]);// log each command line option
    }

    log_text(EVENT_INITIALISATION, "");// insert logical separator

    //execute start server
    if(option.start_server==true){

        if(argc==3) strcpy(db_filename, argv[2]);

        printf("SERVER START using %s at %s on %s\n", db_filename, time_stamp_str, verbose_date_stamp_str);

        log_text(EVENT_INITIALISATION, "SERVER START using %s at %s on %s", db_filename, time_stamp_str, verbose_date_stamp_str);
        log_text(EVENT_INITIALISATION, "");// insert logical separator

        open_database(db_filename);
        start_server();

        return 0;
    }

    //execute create database
    else if(option.create_database==true){

        if(argc==3) strcpy(db_filename, argv[2]);

        printf("CREATE DATABASE using %s at %s on %s\n", db_filename, time_stamp_str, verbose_date_stamp_str);

        log_text(EVENT_INITIALISATION, "CREATE DATABASE using %s at %s on %s", db_filename, time_stamp_str, verbose_date_stamp_str);
        log_text(EVENT_INITIALISATION, "");// insert logical separator

        create_database(db_filename);
        //_create_database("load_script.txt");

        return 0;
    }

    //execute upgrade database
    else if(option.upgrade_database==true){

        if(argc==3) strcpy(db_filename, argv[2]);

        printf("UPGRADE DATABASE using %s at %s on %s\n", db_filename, time_stamp_str, verbose_date_stamp_str);

        log_text(EVENT_INITIALISATION, "UPGRADE DATABASE using %s at %s on %s", db_filename, time_stamp_str, verbose_date_stamp_str);
        log_text(EVENT_INITIALISATION, "");// insert logical separator

        open_database(db_filename);
        upgrade_database(db_filename);

        return 0;
    }

    //execute load map
    else if(option.load_map==true && argc>=9){

        if(argc==10) strcpy(db_filename, argv[9]);

        int map_id=atoi(argv[2]);

        char elm_filename[80]="";
        strcpy(elm_filename, argv[3]);

        open_database(db_filename);

        //load maps so we can find out if a map exists
        load_db_maps();

        if(get_db_map_exists(map_id)==true){

            printf( "Do you wish to replace map [%i] [%s] Y/N: ", map_id, maps.map[map_id].map_name);

            //get decision from stdin
            char decision[10]="";

            if(fgets(decision, sizeof(decision), stdin)==NULL){

                log_event(EVENT_ERROR, "something failed in fgets in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                stop_server();
            }

            //convert decision to upper case and determine if map replacement should proceed
            str_conv_upper(decision);

            if(strncmp(decision, "Y", 1)==0 || strncmp(decision, "YES", 3)==0){

                log_text(EVENT_INITIALISATION, "Replace existing map %i", map_id);
                delete_map(map_id);
            }
            else {

                return 0;
            }

            printf("\n");
        }

        log_text(EVENT_INITIALISATION, "LOAD MAP %i %s on %s at %s on %s", map_id, elm_filename, db_filename, time_stamp_str, verbose_date_stamp_str);
        log_text(EVENT_INITIALISATION, "");// insert logical separator

        add_db_map(map_id, elm_filename);
        add_db_map_objects(map_id, elm_filename);

        return 0;
    }

    //execute list maps
    else if(option.list_maps==true){

        if(argc>2) strcpy(db_filename, argv[2]);

        printf("LIST MAPS using %s at %s on %s\n", db_filename, time_stamp_str, verbose_date_stamp_str);

        log_text(EVENT_INITIALISATION, "LIST MAPS using %s at %s on %s", db_filename, time_stamp_str, verbose_date_stamp_str);
        log_text(EVENT_INITIALISATION, "");// insert logical separator

        open_database(db_filename);
        list_db_maps();

        return 0;
    }

    //display command line options if no command line options are found or command line options are not recognised
    printf("Command line options...\n");
    printf("create database  -C optional [""database file name""]\n");
    printf("start server     -S optional [""database file name""]\n");
    printf("upgrade database -U optional [""database file name""]\n");
    printf("list loaded maps -L optional [""database file name""]\n");
    printf("load map         -M [map id] [""elm filename""]\n");

    return 0;//otherwise we get 'control reached end of non void function'
}
