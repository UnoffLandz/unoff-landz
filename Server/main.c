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

                                    LINKING INFORMATION

To compile server, link with the following libraries :

    libev.a                 - libev event library
    libsqlite3.so           - sqlite database library

****************************************************************************************************/
/***************************************************************************************************

                                TO - DO

convert Uint16 to uint16_t (remove sdl library)
database tables need to be order independent (as (*) causes too many bugs)
test multiple guild application handling
convert elm_file field in MAP TABLE to description
implement map_author, map_author_email, map_status, map_upload_date in MAP_TABLE
#map uploaded date needs implementation
convert db inventory storage to blob and separately record all drops/pick ups in db
db record of chars leaving and joining guilds

finish making push_sql the same as send_text
finish script loading
widen distance that new/beamed chars are from other chars
need #GM guild channel functionality
need #IG guild channel functionality
need #command to give guild details (same as char details)
need OPS #command to #pm all active players (surely this is mod channel???)
need guild channel hello/goodbye messages
need #letter system to inform ppl if guild application has been approved/rejected also if guild member leaves
need #command to change guild tag colour
need #command to change guild description
need #leave_guild
need #command to withdraw application to join guild
need #command to #pm all guild members (guild master only)
implement guild stats

walk to towards bag when clicked on if char is not standing on bag
extend add_client_to_map function so that existing bags are shown to new client
implement pick up bag
implement move item between slots in bag
implement bag poof (include reset poof time on add/drop from bag)

remove character_type_name field from CHARACTER_TYPE_TABLE
map object reserve respawn

save guild applicant list to database
document idle_buffer2.h
convert attribute struct so as attribute type can be addressed programatically
identify cause of stall after login (likely to be loading of inventory from db)
identify cause of char bobbing
put inventory slots in a binary blob (may solve stall on log in)
log illegal use of #jump and other developer #commands
improve error handling on upgrade_database function
refactor function current_database_version
create circular buffer for receiving packets
cope with send not sending all the bytes at once
need #function to describe char and what it is wearing)
document new database/struct relationships
finish char_race_stats and char_gender_stats functions in db_char_tbl.c

***************************************************************************************************/
#define _GNU_SOURCE 1   //supports TEMP_FAILURE_RETRY
#include <stdio.h>      //supports printf function
#include <stdlib.h>     //supports free function
#include <string.h>     //supports memset and strcpy functions
#include <errno.h>      //supports errno function
#include <arpa/inet.h>  //supports recv and accept function
#include <ev.h>         //supports ev event library
#include <fcntl.h>      //supports fcntl
#include <unistd.h>     //supports close function and TEMP_FAILURE_RETRY

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
#include "broadcast_actor_functions.h"
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

#define DEBUG_MAIN 1
#define VERSION "4"

struct ev_io *libevlist[MAX_CLIENTS] = {NULL};

extern int current_database_version();

//declare prototypes
void socket_accept_callback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void socket_read_callback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void timeout_cb(EV_P_ struct ev_timer* timer, int revents);
void timeout_cb2(EV_P_ struct ev_timer* timer, int revents);
void idle_cb(EV_P_ struct ev_idle *watcher, int revents);

void start_server(){

    /** RESULT   : starts the server

        RETURNS  : void

        PURPOSE  : code modularisation

        NOTES    :
    **/

    struct ev_loop *loop = ev_default_loop(0);

    struct ev_io *socket_watcher = (struct ev_io*)malloc(sizeof(struct ev_io));
    struct ev_idle *idle_watcher=(struct ev_idle*)malloc(sizeof(struct ev_idle));
    struct ev_timer *timeout_watcher = (struct ev_timer*)malloc(sizeof(struct ev_timer));
    struct ev_timer *timeout_watcher2 = (struct ev_timer*)malloc(sizeof(struct ev_timer));

    struct sockaddr_in server_addr;

    int sd;

    //set server start time
    game_data.server_start_time=time(NULL);

    //display console message
    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    get_time_stamp_str(game_data.server_start_time, time_stamp_str);
    get_verbose_date_str(game_data.server_start_time, verbose_date_stamp_str);
    printf("SERVER START at %s on %s\n", time_stamp_str, verbose_date_stamp_str);

    //check database version
    int old_version = current_database_version();

    if(old_version != CURRENT_DB_VERSION) {

        printf("Database version [%i] not equal to [%i] - use -U option to upgrade your database\n", old_version, CURRENT_DB_VERSION);
        return;
    }

    //load data from database into memory
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

    //gather initial stats
    get_db_last_char_created(); //loads details of the last char created from the database into the game_data struct
    game_data.char_count=get_db_char_count();

    //create server socket & bind it to socket address
    if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1){

        int errnum=errno;

        log_event(EVENT_ERROR, "socket failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    //clear struct and fill with server socket data
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    log_event(EVENT_INITIALISATION, "setting up server socket on address [%s]: port [%i]", inet_ntoa(server_addr.sin_addr), PORT);

    //allow the socket to be immediately reused
    int bReuseaddr = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char*) &bReuseaddr, sizeof(bReuseaddr)) != 0) {

        int errnum=errno;

        log_event(EVENT_ERROR, "setsockopt failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    //bind the server socket to an address
    if(bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))==-1){

        int errnum=errno;

        log_event(EVENT_ERROR, "bind failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    //listen for incoming client connections
    if(listen(sd, 5)==-1){

        int errnum=errno;

        log_event(EVENT_ERROR, "listen failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));
        stop_server();
    }

    //start the event watchers
    ev_timer_init(timeout_watcher, timeout_cb, 0.05, 0.05);
    ev_timer_start(loop, timeout_watcher);

    ev_timer_init(timeout_watcher2, timeout_cb2, GAME_MINUTE_INTERVAL, GAME_MINUTE_INTERVAL);
    ev_timer_start(loop, timeout_watcher2);

    ev_io_init(socket_watcher, socket_accept_callback, sd, EV_READ);
    ev_io_start(loop, socket_watcher);

    ev_idle_init(idle_watcher, idle_cb);
    ev_idle_start(loop, idle_watcher);

    log_event(EVENT_INITIALISATION, "server initialisation complete");

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

    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    // socket accept: get file description
    client_sd = accept(watcher->fd, (struct sockaddr*) &client_addr, &client_len);
    if (client_sd ==-1) {

        int errnum=errno;

        log_event(EVENT_ERROR, "accept failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "socket [%i] error [%i] [%s]", watcher->fd, errnum, strerror(errnum));
        stop_server();
    }

    // too many connections
    if (client_sd > MAX_CLIENTS) {

        log_event(EVENT_ERROR, "new connection [%i] exceeds client array max [%i] ", client_sd, MAX_CLIENTS);

        //send message to client and deny connection
        send_text(client_sd, CHAT_SERVER, "\nSorry but the server is currently full\n");
        close(client_sd);
        return;
    }

    #if DEBUG_MAIN==1
    printf("client [%i] connected\n", client_sd);
    #endif

    // listen to new client
    ev_io_init(client_watcher, socket_read_callback, client_sd, EV_READ);
    ev_io_start(loop, client_watcher);

    libevlist[client_sd] = client_watcher;

    //set up connection data entry in client struct
    clients.client[client_sd].client_status=CONNECTED;
    strcpy(clients.client[client_sd].ip_address, inet_ntoa(client_addr.sin_addr));

    //set up heartbeat
    gettimeofday(&time_check, NULL);
    clients.client[client_sd].time_of_last_heartbeat=time_check.tv_sec;

    //send welcome message and motd to client
    send_text(client_sd, CHAT_SERVER, SERVER_WELCOME_MSG);
    send_motd(client_sd);
    send_text(client_sd, CHAT_SERVER, "\nHit any key to continue...\n");
}

/*
TEST CODE TO SUPPORT WRITE CALLBACK

void socket_write_callback(struct ev_loop *loop, struct ev_io *watcher, int revents) {

    if(revents & EV_WRITE){

        printf("write\n");
        ev_io_stop(loop, watcher);
    }
}
*/

void socket_read_callback(struct ev_loop *loop, struct ev_io *watcher, int revents) {

    /** RESULT   : handles socket read event

        RETURNS  : void

        PURPOSE  : handles existing client connections

        NOTES    :
    **/

    unsigned char buffer[1024];
    unsigned char packet[1024];
    ssize_t read;

    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(EV_READ & revents){

        //wrapping recv in this macro prevents connection reset by peer errors
        read = TEMP_FAILURE_RETRY(recv(watcher->fd, buffer, 512, 0));

        if (read <0) {

            int errnum=errno;

            log_event(EVENT_ERROR, "read failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            log_text(EVENT_ERROR, "sock [%i] error [%i] [%s]", watcher->fd, errnum, strerror(errnum));

            #if DEBUG_MAIN==1
            printf("read failed for client [%i] with error [%i] [%s]\n", watcher->fd, errnum, strerror(errnum));
            #endif

            log_event(EVENT_SESSION, "closing client [%i] following read error", watcher->fd);

            close_connection_slot(watcher->fd);

            ev_io_stop(loop, libevlist[watcher->fd]);
            free(libevlist[watcher->fd]);
            libevlist[watcher->fd] = NULL;

            //clear the struct
            memset(&clients.client[watcher->fd], '\0', sizeof(clients.client[watcher->fd]));

            return;
        }

        if (read == 0) {

            #if DEBUG_MAIN==1
            printf("client [%i] disconnected\n", watcher->fd);
            #endif

            if (libevlist[watcher->fd]!= NULL) {

                close_connection_slot(watcher->fd);

                ev_io_stop(loop, libevlist[watcher->fd]);
                free(libevlist[watcher->fd]);
                libevlist[watcher->fd] = NULL;

                //clear the struct
                memset(&clients.client[watcher->fd], '\0', sizeof(clients.client[watcher->fd]));
            }
            return;
        }

        //check for data received from client
        if(read>0){

            log_event(EVENT_SESSION, "bytes received [%i]", read);

            //copy new bytes to client packet buffer(memcpy doesn't work)
            int j=0;
            for(j=0; j<read; j++){
                clients.client[watcher->fd].packet_buffer[clients.client[watcher->fd].packet_buffer_length]=buffer[j];
                clients.client[watcher->fd].packet_buffer_length++;
            }

            //if data is in the buffer then read it
            if(clients.client[watcher->fd].packet_buffer_length>0) {

                do {

                    int lsb=clients.client[watcher->fd].packet_buffer[1];
                    int msb=clients.client[watcher->fd].packet_buffer[2];

                    int packet_length=lsb+(msb*256)+2;

                    //update heartbeat
                    clients.client[watcher->fd].time_of_last_heartbeat=time_check.tv_sec;

                    //if insufficient data received then wait for more data
                    if(clients.client[watcher->fd].packet_buffer_length<packet_length) break;

                    //copy packet from buffer
                    for(j=0; j<packet_length; j++){

                        packet[j]=(unsigned char)clients.client[watcher->fd].packet_buffer[j];
                    }

                    //process packet
                    process_packet(watcher->fd, packet);

                    // remove packet from buffer
                    clients.client[watcher->fd].packet_buffer_length=clients.client[watcher->fd].packet_buffer_length-packet_length;

                    for(j=0; j<=clients.client[watcher->fd].packet_buffer_length; j++){
                        clients.client[watcher->fd].packet_buffer[j]=clients.client[watcher->fd].packet_buffer[j+packet_length];
                    }

                } while(1);
            }
        }
/*
        TEST CODE TO SUPPORT WRITE CALLBACK

        ev_io_stop(loop, watcher);
        ev_io_init(watcher, socket_write_callback, watcher->fd, EV_WRITE);
        ev_io_start(loop, watcher);
*/
    }
}


void timeout_cb2(EV_P_ struct ev_timer* timer, int revents){

    /**     RESULT   : handles timeout event

            RETURNS  : void

            PURPOSE  : handles game time updates

            NOTES    :
    **/

    (void)(timer);//removes unused parameter warning
    (void)(loop);

    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    char sql[MAX_SQL_LEN]="";
    game_data.game_minutes++;

    //update game time
    if(game_data.game_minutes>360){

        game_data.game_minutes=0;
        game_data.game_days++;

        snprintf(sql, MAX_SQL_LEN, "UPDATE GAME_DATA_TABLE SET GAME_DAYS=%i WHERE GAME_DATA_ID=1", game_data.game_days);
        push_sql_command(sql);
    }

    snprintf(sql, MAX_SQL_LEN, "UPDATE GAME_DATA_TABLE SET GAME_MINUTES=%i WHERE GAME_DATA_ID=1", game_data.game_minutes);
    push_sql_command(sql);
 }


void timeout_cb(EV_P_ struct ev_timer* timer, int revents){

    /** RESULT   : handles timeout event

        RETURNS  : void

        PURPOSE  : handles fixed interval processing tasks

        NOTES    :
    **/

    (void)(timer);//removes unused parameter warning
    (void)(loop);

    if (EV_ERROR & revents) {

        log_event(EVENT_ERROR, "EV error in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    //update time_check struct
    gettimeofday(&time_check, NULL);

    //check through each connect client and process pending actions
    for(int i=0; i<MAX_CLIENTS; i++){

        //restrict to clients that are logged on or connected
        if(clients.client[i].client_status==LOGGED_IN || clients.client[i].client_status==CONNECTED) {

            //check for lagged connection
            if(clients.client[i].time_of_last_heartbeat+HEARTBEAT_INTERVAL<time_check.tv_sec){

                #if DEBUG_MAIN==1
                printf("Client lagged out [%i] [%s]\n", i, clients.client[i].char_name);
                #endif

                log_event(EVENT_SESSION, "client [%i] char [%s] lagged out", i, clients.client[i].char_name);

                close_connection_slot(i);

                ev_io_stop(loop, libevlist[i]);
                free(libevlist[i]);
                libevlist[i] = NULL;

                memset(&clients.client[i], '\0', sizeof(clients.client[i]));
            }

            //restrict to clients that are logged on
            if(clients.client[i].client_status==LOGGED_IN) {

                //update client game time
                if(clients.client[i].time_of_last_minute+GAME_MINUTE_INTERVAL<time_check.tv_sec){

                    clients.client[i].time_of_last_minute=time_check.tv_sec;
                    send_new_minute(i, game_data.game_minutes);

                    //update database with time char was last in game
                    char sql[MAX_SQL_LEN]="";
                    snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET LAST_IN_GAME=%i WHERE CHAR_ID=%i;",(int)clients.client[i].time_of_last_minute, clients.client[i].character_id);
                    push_sql_command(sql);
                }

                //process any char movements
                process_char_move(i, time_check.tv_usec); //use milliseconds

                //process any harvesting
                process_char_harvest(i, time_check.tv_sec); //use seconds
            }
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

    printf("UnoffLandz Server - version %s\n\n", VERSION);

    if(argc==1){

        printf("command line options...\n");
        printf("-C optional [""database file name""]      ...create database\n");
        printf("-S optional [""database file name""]      ...start server\n");
        printf("-U optional [""database file name""]      ...upgrade database\n");
        printf("-M [map id] [""map name""] [""elm file name""] optional [""database file name""]    ...load map\n");
        printf("-L optional [""database file name""]      ...list maps\n");

        exit(EXIT_FAILURE);
    }

    if (argv[1][0] == '-') {

        const char *db_filename = (argc>2) ? argv[2] : DEFAULT_DATABASE_FILE_NAME;

        //clear logs
        initialise_logs();

        //prepare time stamp
        char time_stamp_str[9]="";
        char verbose_date_stamp_str[50]="";
        get_time_stamp_str(game_data.server_start_time, time_stamp_str);
        get_verbose_date_str(game_data.server_start_time, verbose_date_stamp_str);

        switch(argv[1][1]) {

            case 'S': {//start server

                printf("start server\n");

                log_text(EVENT_INITIALISATION, "SERVER START at %s on %s", time_stamp_str, verbose_date_stamp_str);
                log_text(EVENT_INITIALISATION, "");// insert logical separator

                open_database(db_filename);
                start_server(db_filename);
                break;
            }

            case 'M': {//load map

                printf("load map\n");

                log_text(EVENT_INITIALISATION, "LOAD MAP at %s on %s", time_stamp_str, verbose_date_stamp_str);
                log_text(EVENT_INITIALISATION, "");// insert logical separator

                open_database(db_filename);

                //use intptr_t to prevent int truncation issues when compiled as 64bit
                add_db_map((intptr_t)argv[2], (char*)argv[3], (char*)argv[4]);
                break;
            }

            case  'L': {// list maps

                printf("list maps\n");

                log_text(EVENT_INITIALISATION, "LIST MAPS at %s on %s", time_stamp_str, verbose_date_stamp_str);
                log_text(EVENT_INITIALISATION, "");// insert logical separator

                open_database(db_filename);
                list_db_maps();
                break;
            }

            case 'C': { // create database

                printf("create database\n");

                log_text(EVENT_INITIALISATION, "CREATE DATABASE at %s on %s", time_stamp_str, verbose_date_stamp_str);
                log_text(EVENT_INITIALISATION, "");// insert logical separator

                create_database(db_filename);
                //_create_database("load_script.txt");
                break;
            }

            case 'U': { // upgrade database

                printf("upgrade database\n");

                log_text(EVENT_INITIALISATION, "UPGRADE DATABASE at %s on %s", time_stamp_str, verbose_date_stamp_str);
                log_text(EVENT_INITIALISATION, "");// insert logical separator

                open_database(db_filename);
                upgrade_database(db_filename);
                break;
            }

            default: { //unknown command line option

                printf("unknown command line option [%s]\n", (char*)argv[1]);
            }
        }
    }

    return 0; //otherwise we get 'control reaches end of non-void function
}

