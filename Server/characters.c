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

#include <string.h> //supports strcmp
#include <stdio.h> // supports sprintf

#include "clients.h"
#include "character_race.h"
#include "character_type.h"
#include "game_data.h"
#include "attributes.h"
#include "global.h"
#include "logging.h"
#include "string_functions.h"
#include "server_start_stop.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "characters.h"
#include "gender.h"
#include "date_time_functions.h"
#include "guilds.h"
#include "db/db_character_tbl.h"

struct client_node_type character;

int get_char_visual_range(int actor_node){

    /** public function - see header */

    int race_id=get_char_race_id(actor_node);
    int visual_proximity=0;

    if(game_data.game_minutes < 180){// TODO (themuntdregger#1#): replace hard coded value for number of minutes in game day

        visual_proximity = attribute[race_id].day_vision[clients.client[actor_node].vitality_pp];
    }
    else {

        visual_proximity = attribute[race_id].night_vision[clients.client[actor_node].instinct_pp];
    }

    //prevents problems that arise where visual range attributes are zero
    if(visual_proximity<3) visual_proximity=3;

    return visual_proximity;
}


int char_in_game(char *char_name){

    /** public function - see header */

    //convert char name to upper case
    char target_name[80]="";
    strcpy(target_name, char_name);
    str_conv_upper(target_name);

    char compare_name[80]="";

    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].node_status==CLIENT_NODE_USED &&
            clients.client[i].player_type==PLAYER){

            //convert compare name to upper case
            strcpy(compare_name, clients.client[i].char_name);
            str_conv_upper(compare_name);

            //compare the target name and compare name
            if(strcmp(target_name, compare_name)==0) {

                return i;
            }
        }
    }

    return -1;
}


int char_age(int actor_node){

    /** public function - see header */

    int age=( (int) clients.client[actor_node].time_of_last_minute - (int) clients.client[actor_node].char_created) / (60*60*24);

    return age;
}


void send_char_details(int actor_node, const char *char_name){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    if(get_db_char_data(char_name, -1)==false){

        send_text(socket, CHAT_SERVER, "%c%s", c_red3+127, "character does not exist");
        return;
    }

    send_text(socket, CHAT_SERVER, "%cCharacter    :%s", c_green3+127, character.char_name);

    int race_id=character_type[character.char_type].race_id;
    send_text(socket, CHAT_SERVER, "%cRace         :%s", c_green3+127, race[race_id].race_name);

    int gender_id=character_type[character.char_type].gender_id;
    send_text(socket, CHAT_SERVER, "%cGender       :%s", c_green3+127, gender[gender_id].gender_name);

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";
    get_time_stamp_str(character.char_created, time_stamp_str);
    get_date_stamp_str(character.char_created, date_stamp_str);

    send_text(socket, CHAT_SERVER, "%cDate Created :%s %s", c_green3+127, date_stamp_str, time_stamp_str);

    send_text(socket, CHAT_SERVER, "%cCharacter Age:%i", c_green3+127, char_age(actor_node));

    if(character.guild_id==0){

        send_text(socket, CHAT_SERVER, "%cGuild        :guildless player", c_green3+127);
        send_text(socket, CHAT_SERVER, "%cRank         :n/a", c_green3+127);
        send_text(socket, CHAT_SERVER, "%cJoined       :n/a", c_green3+127);
    }
    else {

        int guild_id=character.guild_id;

        send_text(socket, CHAT_SERVER, "%cGuild        :%s", c_green3+127, guilds.guild[guild_id].guild_name);
        send_text(socket, CHAT_SERVER, "%cGuild tag    :%s", c_green3+127, guilds.guild[guild_id].guild_tag );

        send_text(socket, CHAT_SERVER, "%cRank         :%i", c_green3+127, character.guild_rank);

        get_time_stamp_str(character.joined_guild, time_stamp_str);
        get_date_stamp_str(character.joined_guild, date_stamp_str);

        send_text(socket, CHAT_SERVER, "%cJoined       :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
    }
}
