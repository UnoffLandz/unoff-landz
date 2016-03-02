/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

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

#include <stdio.h> //supports sprintf function
#include <stdlib.h> //support atoi function
#include <string.h> //supports strcmp function

#include "string_functions.h"
#include "logging.h"
#include "clients.h"
#include "characters.h"
#include "character_race.h"
#include "colour.h"
#include "server_messaging.h"
#include "maps.h"
#include "movement.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "gender.h"
#include "character_type.h"
#include "chat.h"
#include "game_data.h"
#include "guilds.h"
#include "idle_buffer2.h"
#include "hash_commands.h"
#include "hash_commands_guilds.h"
#include "hash_commands_chat.h"
#include "hash_commands_ops.h"
#include "hash_commands_devs.h"
#include "boats.h"

static int hash_motd(int actor_node, char *text) {

    /** RESULT  : handles motd hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    (void)(text);

    int socket=clients.client[actor_node].socket;
    send_motd_file(socket);
    return 0;
}


static int hash_char_details(int actor_node, char *text) {

    /** RESULT  : handles details hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    char char_name[MAX_CHAR_NAME_LEN]="";
    int socket=clients.client[actor_node].socket;

    if(sscanf(text, "%*s %s", char_name)==-1){

        // if char name not specified, substitute with calling char name
        push_command(actor_node, IDLE_BUFFER_PROCESS_HASH_DETAILS, clients.client[actor_node].char_name, 0);
        return 0;
    }
    else if(sscanf(text, "%*s %s", char_name)==1){

        // if char name is specified use that name
        push_command(actor_node, IDLE_BUFFER_PROCESS_HASH_DETAILS, char_name, 0);
        return 0;
    }

    send_text(socket, CHAT_SERVER, "%c%s", c_red3+127, "you need to use the format #CHAR_DETAILS [character name] or #CD [character name");

    return 0;
}


static int hash_beam_me(int actor_node, char *text) {

    /** RESULT  : handles #beam_me hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char tail[80];

    if(sscanf(text, "%*s %s", tail)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #BEAM ME", c_red3+127);
        return 0;
    }

    str_conv_upper(tail);

    if(strcmp(tail, "ME")!=0){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #BEAM ME", c_red3+127);
        return 0;
    }

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[actor_node].path_count=0;

    //ensure char doesn't beam on top of another char
    int new_map_tile=get_nearest_unoccupied_tile(game_data.beam_map_id, game_data.beam_map_tile);

    //beam the char
    move_char_between_maps(actor_node, game_data.beam_map_id, new_map_tile);

    //if beaming from boat, show no longer on board
    clients.client[actor_node].on_boat=false;

    send_text(socket, CHAT_SERVER, "%cScotty beamed you up", c_green3+127);
    return 0;
}


static int hash_locate_me(int actor_node, char *text) {

    /** RESULT  : tells char map information

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    (void)(text);

    get_map_details(actor_node, clients.client[actor_node].map_id);

    return 0;
}


static int hash_boat(int actor_node, char *text) {

    /** RESULT  : lists available maps

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    (void)(text);

    int socket=clients.client[actor_node].socket;

    if(clients.client[actor_node].boat_booked==true){

        int boat_node=clients.client[actor_node].boat_node;
        int map_id=boat[boat_node].departure_map_id;

        send_text(socket, CHAT_SERVER, "%cyou're booked on the %02i:%02i to %s.",
        c_green3+127,
        clients.client[actor_node].boat_departure_time / 60,
        clients.client[actor_node].boat_departure_time % 60,
        maps.map[map_id].map_name);
    }
    else {

        send_text(socket, CHAT_SERVER, "%cyou've not booked a boat.", c_green3+127);
    }

    return 0;
}


typedef int (*hash_command_function)(int actor_node, char *text);


struct hash_command_array_entry {

    char command[80];
    bool guild_command;
    int guild_rank;
    int permission_level;
    hash_command_function fn;
};


struct hash_command_array_entry hash_command_entries[] = {

//                                     Guild    Permission
//   Hash Command               Guild  Rank     Level         Function
//   -------------------------- ------ -------- ------------  --------
    {"#JC",                     false, 0,       PERMISSION_1, hash_jc},
    {"#JOIN_CHANNEL",           false, 0,       PERMISSION_1, hash_jc},
    {"#LC",                     false, 0,       PERMISSION_1, hash_lc},
    {"#LEAVE_CHANNEL",          false, 0,       PERMISSION_1, hash_lc},
    {"#MOTD",                   false, 0,       PERMISSION_1, hash_motd},
    {"#MESSAGE_OF_THE_DAY",     false, 0,       PERMISSION_1, hash_motd},
    {"#CL",                     false, 0,       PERMISSION_1, hash_cl},
    {"#CHANNEL_LIST",           false, 0,       PERMISSION_1, hash_cl},
    {"#CP",                     false, 0,       PERMISSION_1, hash_cp},
    {"#CHANNEL_PARTICIPANTS",   false, 0,       PERMISSION_1, hash_cp},
    {"#CHAR_DETAILS",           false, 0,       PERMISSION_1, hash_char_details},
    {"#CD",                     false, 0,       PERMISSION_1, hash_char_details},
    {"#BEAM",                   false, 0,       PERMISSION_1, hash_beam_me},
    {"#PM",                     false, 0,       PERMISSION_1, hash_pm},
    {"#PRIVATE_MESSAGE",        false, 0,       PERMISSION_1, hash_pm},
    {"#JUMP",                   true, 0,        PERMISSION_2, hash_jump},
    {"#AG",                     false, 0,       PERMISSION_1, hash_apply_guild},
    {"#APPLY_GUILD",            false, 0,       PERMISSION_1, hash_apply_guild},
    {"#OPS_CREATE_GUILD",       true,  0,       PERMISSION_3, hash_ops_create_guild},
    {"#OPS_APPOINT_GUILD_MEMBER", true,  0,     PERMISSION_3, hash_ops_appoint_guild_member},
    {"#CR",                      true,  18,     PERMISSION_1, hash_change_rank},
    {"#CHANGE_RANK",             true,  18,     PERMISSION_1, hash_change_rank},
    {"#OPS_APPOINT_GUILD_MEMBER", true, 0,      PERMISSION_3, hash_ops_appoint_guild_member},
    {"#OPS_CHANGE_GUILD_MEMBER_RANK", true, 0,  PERMISSION_3, hash_ops_change_guild_member_rank},
    {"#OPS_CHANGE_GUILD_PERMISSION",true, 20,   PERMISSION_3, hash_ops_change_guild_permission},
    {"#LA",                      true,   18,    PERMISSION_1, hash_list_applicants},
    {"#LIST_APPLICANTS",         true,   18,    PERMISSION_1, hash_list_applicants},
    {"#AA",                      true,   18,    PERMISSION_1, hash_accept_applicant},
    {"#ACCEPT_APPLICANT",        true,   18,    PERMISSION_1, hash_accept_applicant},
    {"#RA",                      true,   18,    PERMISSION_1, hash_reject_applicant},
    {"#REJECT_APPLICANT",        true,   18,    PERMISSION_1, hash_reject_applicant},
    {"#KM",                      true,   18,    PERMISSION_1, hash_kick_guild_member},
    {"#KICK_GUILD_MEMBER",       true,   18,    PERMISSION_1, hash_kick_guild_member},
    {"#OPS_KICK_GUILD_MEMBER",   true,   18,    PERMISSION_3, hash_ops_kick_guild_member},
    {"#LIST_GUILD",              true,   0,     PERMISSION_1, hash_list_guild},
    {"#LG",                      true,   0,     PERMISSION_1, hash_list_guild},
    {"#LOCATE_ME",               false,  0,     PERMISSION_1, hash_locate_me},
    {"#MAP",                     false,  0,     PERMISSION_1, hash_map},
    {"#LIST_MAPS",               false,  0,     PERMISSION_2, hash_list_maps},
    {"#GUILD_DETAILS",           false,  0,     PERMISSION_1, hash_guild_details},
    {"#GD",                      false,  0,     PERMISSION_1, hash_guild_details},
    {"#GUILD_MESSAGE",           true,   0,     PERMISSION_1, hash_guild_message},
    {"#GM",                      true,   1,     PERMISSION_1, hash_guild_message},
    {"#SET_GUILD_DESCRIPTION",   true,   18,    PERMISSION_1, hash_set_guild_description},
    {"#SD",                      true,   18,    PERMISSION_1, hash_set_guild_description},
    {"#SET_GUILD_TAG_COLOUR",    true,   18,    PERMISSION_1, hash_set_guild_tag_colour},
    {"#SC",                      true,   18,    PERMISSION_1, hash_set_guild_tag_colour},
    {"#LEAVE_GUILD",             true,   0,     PERMISSION_1, hash_leave_guild},
    {"#LG",                      true,   0,     PERMISSION_1, hash_leave_guild},
    {"#SERVER_MESSAGE",          true,   0,     PERMISSION_2, hash_server_message},
    {"#SM",                      true,   0,     PERMISSION_2, hash_server_message},
    {"#TRACK",                   true,   0,     PERMISSION_2, hash_track},
    {"#TRACE",                   true,   0,     PERMISSION_2, hash_trace},
    {"#TRACE_EXPLORE",           true,   0,     PERMISSION_2, hash_trace_explore},
    {"#TRACE_PATH",              true,   0,     PERMISSION_2, hash_trace_path},
    {"#BOAT",                    false,  0,     PERMISSION_1, hash_boat},
    {"#SET_MAP_NAME",            true,   0,     PERMISSION_2, hash_set_map_name},
    {"#SET_MAP_DESCRIPTION",     true,   0,     PERMISSION_2, hash_set_map_description},
    {"#SET_MAP_AUTHOR",          true,   0,     PERMISSION_2, hash_set_map_author},
    {"#SET_MAP_AUTHOR_EMAIL",    true,   0,     PERMISSION_2, hash_set_map_author_email},
    {"#SET_MAP_STATUS",          true,   0,     PERMISSION_2, hash_set_map_development_status},
    {"#OBJECT",                  true,   0,     PERMISSION_3, hash_object},
    {"", false, 0, 0, 0}
};


static const struct hash_command_array_entry *find_hash_command_entry(char *command) {

    /** RESULT  : finds hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    int i=0;

    while(hash_command_entries[i].fn!=NULL){

        if(strcmp(hash_command_entries[i].command, command)==0){

            return &hash_command_entries[i];
        }

        ++i;
    }

    return NULL;
}


void process_hash_commands(int actor_node, char *text){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    //grab the first part of the text string as this contains the hash command name
    char hash_command[80]="";
    sscanf(text, "%s", hash_command);
    str_conv_upper(hash_command);

    //lookup the hash command function
    const struct hash_command_array_entry *hash_command_entry = find_hash_command_entry(hash_command);

    //check if hash command exists
    if(!hash_command_entry){

        send_text(socket, CHAT_SERVER, "%ccommand %s is not supported", c_red3+127, hash_command);
        log_event(EVENT_SESSION, "unknown #command [%s]", hash_command);
        return;
    }

    //prevent guildless players from using guild commands
    int guild_id=clients.client[actor_node].guild_id;

    if(hash_command_entry->guild_command==true && guild_id==0){

        send_text(socket, CHAT_SERVER, "%cCommand can only be used by guild members", c_red3+127 );
        return;
    }

    //prevent guild players for using commands above their rank
    int required_rank=hash_command_entry->guild_rank;

    if(guild_id>0 && required_rank>clients.client[actor_node].guild_rank){

        send_text(socket, CHAT_SERVER, "%cCommand can only be used by guild members with rank %i or above", c_red3+127, required_rank);
        return;
    }

    //prevent guilds from using commands above their permission
    int permission_level=guilds.guild[guild_id].permission_level;

    if(guild_id>0 && hash_command_entry->permission_level>permission_level){

        send_text(socket, CHAT_SERVER, "%cYou are not authorised to use that command", c_red3+127);
        return;
    }

    //execute the hash command
    hash_command_entry->fn(actor_node, text);

    log_event(EVENT_SESSION, "client [%i] character [%s] command [%s]", actor_node, clients.client[actor_node].char_name, text);
}
