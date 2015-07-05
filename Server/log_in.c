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

#include <stdio.h> //support for sprintf
#include <string.h> //support for strcpy

#include "log_in.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "string_functions.h"
#include "logging.h"
#include "server_messaging.h"
#include "global.h"
#include "clients.h"
#include "characters.h"
#include "db/db_character_tbl.h"
#include "maps.h"
#include "server_start_stop.h"
#include "game_data.h"

void load_char_data_into_connection(int connection){

    clients.client[connection].character_id=character.character_id;
    strcpy(clients.client[connection].char_name, character.char_name);
    strcpy(clients.client[connection].password, character.password);
    clients.client[connection].char_status=character.char_status;
    clients.client[connection].active_chan=character.active_chan;
    clients.client[connection].chan[0]=character.chan[0];
    clients.client[connection].chan[1]=character.chan[1];
    clients.client[connection].chan[2]=character.chan[2];
    clients.client[connection].gm_permission=character.gm_permission;
    clients.client[connection].ig_permission=character.ig_permission;
    clients.client[connection].map_id=character.map_id;
    clients.client[connection].map_tile=character.map_tile;
    clients.client[connection].guild_id=character.guild_id;
    clients.client[connection].char_type=character.char_type;
    clients.client[connection].skin_type=character.skin_type;
    clients.client[connection].hair_type=character.hair_type;
    clients.client[connection].shirt_type=character.shirt_type;
    clients.client[connection].pants_type=character.pants_type;
    clients.client[connection].boots_type=character.boots_type;
    clients.client[connection].head_type=character.head_type;
    clients.client[connection].shield_type=character.shield_type;
    clients.client[connection].weapon_type=character.weapon_type;
    clients.client[connection].cape_type=character.cape_type;
    clients.client[connection].helmet_type=character.helmet_type;
    clients.client[connection].frame=character.frame;
    clients.client[connection].max_health=character.max_health;
    clients.client[connection].current_health=character.current_health;
    clients.client[connection].char_created=character.char_created;
    clients.client[connection].joined_guild=character.joined_guild;

    int i=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){
        clients.client[connection].client_inventory[i].image_id=character.client_inventory[i].image_id;
        clients.client[connection].client_inventory[i].amount=character.client_inventory[i].amount;
    }

    clients.client[connection].overall_exp=character.overall_exp;
    clients.client[connection].harvest_exp=character.harvest_exp;
}


void process_log_in(int connection, const unsigned char *packet){

    /** public function - see header **/

    char text[1024]="";
    char text_out[1024]="";
    int map_id=0;
    //int chan_colour=0;

    int packet_length=packet[1]+(packet[2]*256)-1+3;

    int i=0;
    for(i=3; i<packet_length; i++){
        text[i-3]=packet[i];
        if(packet[i]==ASCII_NULL) break;
    }

    //check that the login packet is correct
    if(count_str_island(text)!=2){

        sprintf(text_out, "%cSorry, but that caused an error", c_red1+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        send_login_not_ok(connection);
        log_event(EVENT_ERROR, "malformed login attempt [%s]", text);

        return;
    }

    //Extract the char name and password from the login packet
    char char_name[1024]="";
    char password[1024]="";

    get_str_island(text, char_name, 1);
    get_str_island(text, password, 2);

    log_event(EVENT_SESSION, "login attempt char name [%s] password [%s]", char_name, password);

    //get the char_id corresponding to the char name
    int char_id=get_db_char_data(char_name);

    if(char_id==NOT_FOUND) {

        send_you_dont_exist(connection);
        send_raw_text(connection, CHAT_SERVER, "unknown character name");

        log_event(EVENT_SESSION, "login rejected - unknown char name");

        return;
    }

    //the get_char_data function loads the char data into a temporary struct, so now we use the load_char_data
    //function to transfer that data into the client struct
    load_char_data_into_connection(connection);

    //check we have the correct password for our char
    if(strcmp(password, clients.client[connection].password)!=0){

        send_login_not_ok(connection);
        log_event(EVENT_SESSION, "login rejected - incorrect password");

        return;
    }

    //prevent login of dead/banned chars
    if(clients.client[connection].char_status!=CHAR_ALIVE){

        switch(clients.client[connection].char_status){

            case CHAR_DEAD:
            log_event(EVENT_SESSION, "login rejected - dead char");
            break;

            case CHAR_BANNED:
            log_event(EVENT_SESSION, "login rejected - banned char");
            break;

            default:
            log_event(EVENT_ERROR, "login rejected - unknown char status");
        }

        send_login_not_ok(connection);
        return;
    }

    //prevent concurrent login on same char
    for(i=1; i<MAX_CLIENTS; i++){

        if(clients.client[connection].character_id==clients.client[i].character_id \
           && clients.client[connection].client_status==LOGGED_IN \
           && i!=connection){

            send_login_not_ok(connection);
            log_event(EVENT_SESSION, "concurrent login attempt for char [%s]", char_name);

            return;
        }
    }

    clients.client[connection].client_status=LOGGED_IN;
    log_event(EVENT_SESSION, "login accepted");

/*
    // notify guild that char has logged on
    int guild_id=clients.client[connection].guild_id;

    if(guild_id>0) {

        chan_colour=guilds.guild[guild_id]->log_on_notification_colour;
        sprintf(text_out, "%c%s JOINED THE GAME", chan_colour, clients.client[connection].char_name);
        broadcast_guild_channel_chat(guild_id, text_out);
    }
*/
    //add char to map (makes scene visible in client)
    map_id=clients.client[connection].map_id;

    if(add_char_to_map(connection, map_id, clients.client[connection].map_tile)==ILLEGAL_MAP){

        log_event(EVENT_ERROR, "cannot add char [%s] to map [%s] in function %s: module %s: line %i", char_name, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //record when session commenced so we can calculate time in-game
    clients.client[connection].session_commenced=time(NULL);

    send_login_ok(connection);
    send_you_are(connection);
    send_get_active_channels(connection);
    send_here_your_stats(connection);
    send_here_your_inventory(connection);
    //send_new_minute(connection, game_data.game_minutes);
}
