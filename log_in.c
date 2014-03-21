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

void load_char_data_into_connection(int connection){

    clients.client[connection]->character_id=character.char_id;
    strcpy(clients.client[connection]->char_name, character.char_name);
    strcpy(clients.client[connection]->password, character.password);
    clients.client[connection]->char_status=character.char_status;
    clients.client[connection]->active_chan=character.active_chan;
    clients.client[connection]->chan[0]=character.chan[0];
    clients.client[connection]->chan[1]=character.chan[1];
    clients.client[connection]->chan[2]=character.chan[2];
    clients.client[connection]->gm_permission=character.gm_permission;
    clients.client[connection]->ig_permission=character.ig_permission;
    clients.client[connection]->map_id=character.map_id;
    clients.client[connection]->map_tile=character.map_tile;
    clients.client[connection]->guild_id=character.guild_id;
    clients.client[connection]->char_type=character.char_type;
    clients.client[connection]->skin_type=character.skin_type;
    clients.client[connection]->hair_type=character.hair_type;
    clients.client[connection]->shirt_type=character.shirt_type;
    clients.client[connection]->pants_type=character.pants_type;
    clients.client[connection]->boots_type=character.boots_type;
    clients.client[connection]->head_type=character.head_type;
    clients.client[connection]->shield_type=character.shield_type;
    clients.client[connection]->weapon_type=character.weapon_type;
    clients.client[connection]->cape_type=character.cape_type;
    clients.client[connection]->helmet_type=character.helmet_type;
    clients.client[connection]->frame=character.frame;
    clients.client[connection]->max_health=character.max_health;
    clients.client[connection]->current_health=character.current_health;
    clients.client[connection]->visual_proximity=character.visual_proximity;
    clients.client[connection]->local_text_proximity=character.local_text_proximity;
    clients.client[connection]->last_in_game=character.last_in_game;
    clients.client[connection]->char_created=character.char_created;
    clients.client[connection]->joined_guild=character.joined_guild;

    clients.client[connection]->inventory_length=character.inventory_length;
    memcpy(clients.client[connection]->inventory, character.inventory, character.inventory_length);

    clients.client[connection]->overall_exp=character.overall_exp;
    clients.client[connection]->harvest_exp=character.harvest_exp;
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

    //check that the login packet is correct
    if(count_str_island(text)!=2){

        sprintf(text_out, "%cSorry, but that caused an error", c_red1+127);
        send_server_text(connection, CHAT_SERVER, text_out);

        send_login_not_ok(connection);

        sprintf(text_out, "malformed login attempt for existing char name [%s] password [%s]\n", char_name, password);
        log_event(EVENT_ERROR, text_out);
        return;
    }

    //Extract the char name and password from the login packet
    get_str_island(text, char_name, 1);
    get_str_island(text, password, 2);

    sprintf(text_out, "login char name [%s] password [%s]\n", char_name, password);
    log_event(EVENT_SESSION, text_out);

    //get the char_id corresponding to the char name
    char_id=get_char_data(char_name);

    if(char_id==NOT_FOUND) {

        send_you_dont_exist(connection);
        log_event(EVENT_SESSION, "login rejected - unknown char name\n");
        send_server_text(connection, CHAT_SERVER, "unknown character name");
        return;
    }

    //the get_char_data function loads the char data into a temporary struct, so now we use the load_char_data function
    //to transfer that data into the connection struct
    load_char_data_into_connection(connection);

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
    for(i=1; i<clients.max; i++){

        if(clients.client[connection]->character_id==clients.client[i]->character_id \
           && clients.client[connection]->status==LOGGED_IN \
           && i!=connection){

            send_login_not_ok(connection);

            sprintf(text_out, "concurrent login attempt for char [%s]\n", char_name);
            log_event(EVENT_SESSION, text_out);
            return;
        }
    }

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


    //tell char to sit if the frame is set to sit
    if(clients.client[connection]->frame==sit_down) {
        printf("tell char to sit\n");
        broadcast_actor_packet(connection, sit_down, clients.client[connection]->map_tile);
    }
    if(clients.client[connection]->frame==stand_up) {
        printf("tell char to stand\n");
        broadcast_actor_packet(connection, stand_up, clients.client[connection]->map_tile);
    }

    send_login_ok(connection);
    send_you_are(connection);
    send_get_active_channels(connection);
    send_here_your_stats(connection);
    send_here_your_inventory(connection);

    sprintf(text_out, "login succesful char [%s]\n", char_name);
    log_event(EVENT_SESSION, text_out);
}
