#include <stdio.h>
#include <stdlib.h> // needed for EXIT_FAILURE
#include <sys/socket.h> //needed for send function
#include <sys/time.h> //needed for usec time
#include <string.h>

#include "global.h"
#include "protocol.h"
#include "log_in.h"
#include "maps.h"
#include "string_functions.h"
#include "character_movement.h"
#include "files.h"
#include "chat.h"
#include "broadcast.h"
#include "database.h"

int is_char_concurrent(int connection){

     /** RESULT  : checks is a char is concurrently logged to the server

        RETURNS : 0=non-concurrent / -1=concurrent

        PURPOSE : prevent concurrent logins on the same char

        USAGE   : protocol.c process_packet
    */

    int i=0;
    int char_id=clients.client[connection]->character_id;
    int char_count=0;

    for(i=0; i<clients.max; i++){

        if(clients.client[i]->character_id==char_id && i!=connection){
            char_count++;
            if(char_count>0) return CHAR_CONCURRENT;
        }
    }

    return CHAR_NON_CONCURRENT;
}

void send_login_ok(int connection){

    unsigned char packet[3];

    packet[0]=250;
    packet[1]=1;
    packet[2]=0;

    send(connection, packet, 3, 0);
}

void send_login_not_ok(int connection){

    unsigned char packet[3];

    packet[0]=251;
    packet[1]=1;
    packet[2]=0;

    send(connection, packet, 3, 0);
}

void send_you_dont_exist(int connection){

    unsigned char packet[3];

    packet[0]=249;
    packet[1]=1;
    packet[2]=0;

    send(connection, packet, 3, 0);
}

void send_you_are(int connection){

    unsigned char packet[5];
    int id_msb=connection / 256;
    int id_lsb=connection % 256;

    packet[0]=3;
    packet[1]=3;
    packet[2]=0;
    packet[3]=id_lsb;
    packet[4]=id_msb;

    send(connection, packet, 5, 0);
}

void process_log_in(int connection, char *text) {

    char char_name[1024]="";
    char password[1024]="";
    char text_out[1024]="";
    int char_id=0;
    int map_id=0;
    int guild_id=0;
    int chan_colour=0;
    int i=0;

    if(count_str_island(text)!=2){

        printf("login not ok\n");

        sprintf(text_out, "%cSorry, but that caused an error", c_red1+127);
        send_server_text(connection, CHAT_SERVER, text_out);

        send_login_not_ok(connection);

        sprintf(text_out, "malformed login attempt for existing char name [%s] password [%s]\n", char_name, password);
        log_event(EVENT_ERROR, text_out);
        return;
    }

    get_str_island(text, char_name, 1);
    get_str_island(text, password, 2);

    sprintf(text_out, "login char name [%s] password [%s]\n", char_name, password);
    log_event(EVENT_SESSION, text_out);

    //now we have the char name, get the char id
    char_id=get_char_data(char_name);

    if(char_id==CHAR_NOT_FOUND) {

        send_you_dont_exist(connection);
        log_event(EVENT_SESSION, "login rejected - unknown char name\n");
        return;
    }

    //load char from database into the client struct array
    /***could replace this with a transfer from the character struct as the get_char_data function
    will already have loaded the char data to the character struct and this would save having to query
    the db twice */
    load_character_from_database(char_id, connection);

    //check we have the correct password for our char
    if(strcmp(password, clients.client[connection]->password)==PASSWORD_INCORRECT){

        send_login_not_ok(connection);
        log_event(EVENT_SESSION, "login rejected - incorrect password\n");
        return;
    }

    //prevent login of dead/banned chars
    if(clients.client[connection]->char_status!=CHAR_ALIVE){

        switch(clients.client[connection]->char_status){

            case CHAR_DEAD:
            log_event(EVENT_SESSION, "login rejected - dead char\n");
            break;

            case CHAR_BANNED:
            log_event(EVENT_SESSION, "login rejected - banned char\n");
            break;
        }

        send_login_not_ok(connection);
        return;
    }

    //prevent concurrent login on same char
    /*
    if(is_char_concurrent(connection)==CHAR_CONCURRENT){

        printf("char concurrent\n");
        send_login_not_ok(connection);

        sprintf(text_out, "concurrent login attempt for char [%s]\n", char_name);
        log_event(EVENT_SESSION, text_out);
        return;
    }
    */

    clients.client[connection]->status=LOGGED_IN;

    //add char to local channel lists
    for(i=0; i<3; i++){
        if(clients.client[connection]->chan[i]>0) add_client_to_channel(connection, clients.client[connection]->chan[i]);
    }

    // notify guild that char has logged on
    guild_id=clients.client[connection]->guild_id;

    if(guild_id>0) {

        chan_colour=guilds.guild[guild_id]->log_on_notification_colour;
        sprintf(text_out, "%c%s JOINED THE GAME", chan_colour, clients.client[connection]->char_name);
        broadcast_guild_channel_chat(guild_id, text_out);
    }

    //add char to local map list
    map_id=clients.client[connection]->map_id;

    if(add_char_to_map(connection, map_id, clients.client[connection]->map_tile)==ILLEGAL_MAP){

        sprintf(text_out, "cannot add char [%s] to map [%s] in function process_packet", char_name, maps.map[map_id]->map_name);
        log_event(EVENT_ERROR, text_out);

        exit(EXIT_FAILURE);
    }

    send_login_ok(connection);
    send_you_are(connection);
    send_get_active_channels(connection);
    //send_here_your_stats(connection);
    //send_here_your_inventory(connection);

    sprintf(text_out, "login succesful char [%s]\n", char_name);
    log_event(EVENT_SESSION, text_out);
}
