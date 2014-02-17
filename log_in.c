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

int validate_password(int char_id, char *password){

    if(strcmp(password, characters.character[char_id]->password)==0) return PASSWORD_CORRECT;

    return PASSWORD_INCORRECT;
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

void send_you_are(int connection, int id){

    unsigned char packet[5];
    int id_msb=id / 256;
    int id_lsb=id % 256;

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

    printf("character_name[%s] password[%s]\n", char_name, password);

    //now we have the char name, get the char id
    char_id=get_char_id(char_name);

    if(char_id==0) {

        send_you_dont_exist(connection);

        sprintf(text_out, "login attempt with unknown char name [%s] password [%s]\n", char_name, password);
        log_event(EVENT_SESSION, text_out);
        return;
    }

    //check we have the correct password for our char
    if(validate_char_password(char_id, password)==0){

        send_login_not_ok(connection);

        sprintf(text_out, "login attempt with incorrect password char name [%s] password [%s]\n", char_name, password);
        log_event(EVENT_SESSION, text_out);
        return;
    }


    //need to transfer char data from database into client struct












    //prevent login of dead chars
    if(characters.character[char_id]->char_status==CHAR_DEAD) {

        send_login_not_ok(connection);

        sprintf(text_out, "login attempt for dead char [%s]\n", char_name);
        log_event(EVENT_SESSION, text_out);
        return;
    }

    //prevent login of banned chars
    if(characters.character[char_id]->char_status==CHAR_BANNED) {

        send_login_not_ok(connection);

        sprintf(text_out, "login attempt for locked char [%s]\n", char_name);
        log_event(EVENT_SESSION, text_out);
        return;
    }

//now we have the char id, get the map id for the char
    map_id=characters.character[char_id]->map_id;

    //link char to client (do it here as it is needed for the is_char_concurrent function)
    clients.client[connection]->character_id=char_id;

    //prevent concurrent login on same char
    if(is_char_concurrent(connection)==CHAR_CONCURRENT){

        printf("char concurrent\n");
        send_login_not_ok(connection);

        sprintf(text_out, "concurrent login attempt for char [%s]\n", char_name);
        log_event(EVENT_SESSION, text_out);
        return;
    }

    clients.client[connection]->status=LOGGED_IN;

    //add char to local channel lists
    for(i=0; i<3; i++){
        if(characters.character[char_id]->chan[i]>0) add_client_to_channel(connection, characters.character[char_id]->chan[i]);
    }

    // notify guild that char has logged on
    guild_id=characters.character[char_id]->guild_id;

    if(guild_id>0) {
        chan_colour=guilds.guild[guild_id]->log_on_notification_colour;
        sprintf(text_out, "%c%s JOINED THE GAME", chan_colour, characters.character[char_id]->char_name);
        broadcast_guild_channel_chat(guild_id, text_out);
    }

    //add char to local map list
    if(add_char_to_map(connection, map_id, characters.character[char_id]->map_tile)==ILLEGAL_MAP){

        sprintf(text_out, "cannot add char [%s] to map [%s] in function process_packet", char_name, maps.map[map_id]->map_name);
        log_event(EVENT_ERROR, text_out);

        perror(text_out);
        exit(EXIT_FAILURE);
    }

    //confirm login to client
    send_login_ok(connection);
    send_you_are(connection, clients.client[connection]->character_id);
    send_get_active_channels(connection);
    send_here_your_stats(connection);
    //send_here_your_inventory(connection);

    sprintf(text_out, "login succesful char [%s]\n", char_name);
    log_event(EVENT_SESSION, text_out);
}
