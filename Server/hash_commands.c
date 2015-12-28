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
#include "broadcast_chat.h"
#include "hash_commands.h"
#include "hash_commands_guilds.h"
#include "hash_commands_chat.h"

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

    char char_name[80]="";
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
    int new_map_tile=get_nearest_unoccupied_tile(actor_node, game_data.beam_map_id, game_data.beam_map_tile);

    //beam the char
    move_char_between_maps(actor_node, game_data.beam_map_id, new_map_tile);

    //if beaming from boat, show no longer on board
    clients.client[actor_node].on_boat=false;

    send_text(socket, CHAT_SERVER, "%cScotty beamed you up", c_green3+127);
    return 0;
}


static int hash_jump(int actor_node, char *text) {

    /** RESULT  : jumps char to a new map at (x/y)cartesian coordinates

        RETURNS : void

        PURPOSE : allows map developers to jump to unconnected maps

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    int map_id, x, y;

    if(sscanf(text, "%*s %i %i %i", &map_id, &x, &y)!=3){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #JUMP [map id] [x] [y]", c_red3+127);
        return 0;
    }

    //check that coordinates are in bounds
    int map_axis=maps.map[map_id].map_axis;
    if(x>map_axis || y>map_axis || x<0 || y<0){

        send_text(socket, CHAT_SERVER, "%ccordinates must be between 0 and %i", c_red3+127, map_axis);
        return 0;
    }

    //calculate map tile
    int map_tile=get_tile(x, y, map_id);

    //move char
    if(move_char_between_maps(actor_node, map_id, map_tile)==false){

        send_text(socket, CHAT_SERVER, "%cjump failed", c_red3+127);
        return 0;
    }

    send_text(socket, CHAT_SERVER, "%cYou jumped to map %s tile %i", c_green3+127, maps.map[map_id].map_name, map_tile);

    return 0;
}


static int hash_ops_create_guild(int actor_node, char *text) {

    /** RESULT  : OPS command to create guild

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char guild_name[80];
    char guild_tag[GUILD_TAG_LENGTH];
    int permission_level=0;

    if(sscanf(text, "%*s %s %s %i", guild_name, guild_tag, &permission_level)!=3){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OPS_CREATE_GUILD [guild name][guild tag][permission level]", c_red3+127);
        return 0;
    }

    create_guild(socket, guild_name, guild_tag, permission_level);
    return 0;
}


static int hash_ops_appoint_guild_member(int actor_node, char *text) {

    /** RESULT  : OPS command to appoint a player to a guild

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char char_name[80]="";
    char guild_tag[GUILD_TAG_LENGTH]="";

    if(sscanf(text, "%*s %s %s", char_name, guild_tag)!=2){

        send_text(socket, CHAT_SERVER, "you need to use the format #OPS_APPOINT_GUILD_MEMBER [character name][guild tag]", c_red3+127);
        return 0;
    }

    join_guild(actor_node, char_name, guild_tag);
    return 0;
}


static int hash_server_message(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char message[1024]="";

    //scans the string from the second element onwards
    if(sscanf(text, "%*s %[^\n]", message)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SERVER_MESSAGE [message] or #SM [message]", c_red3+127);
        return 0;
    }

    //broadcast message to all clients
    broadcast_server_message(message);

    return 0;
}


static int hash_ops_change_guild_member_rank(int actor_node, char *text) {

    /** RESULT  : change a chars guild rank

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char char_name[80]="";
    char guild_tag[GUILD_TAG_LENGTH]="";
    int guild_rank=0;

    if(sscanf(text, "%*s %s %s %i", char_name, guild_tag, &guild_rank)!=3){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OPS_CHANGE_GUILD_MEMBER_RANK [character_name][guild_tag][rank]", c_red3+127);
        return 0;
    }

    change_guild_rank(actor_node, char_name, guild_tag, guild_rank);
    return 0;
}


static int hash_ops_change_guild_permission(int actor_node, char *text) {

    /** RESULT  : change a guilds permission level

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char guild_tag[GUILD_TAG_LENGTH]="";
    int permission_level=0;

    if(sscanf(text, "%*s %s %i", guild_tag, &permission_level)!=2){

        send_text(socket, CHAT_SERVER, "you need to use the format #OPS_CHANGE_GUILD_PERMISSION [guild tag][permission level]", c_red3+127);
        return 0;
    }

    change_guild_permission(actor_node, guild_tag, permission_level);
    return 0;
}


static int hash_ops_kick_guild_member(int actor_node, char *text) {

    /** RESULT  : ops kicks a member from a a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char char_name[80]="";
    char guild_tag[GUILD_TAG_LENGTH]="";

    if(sscanf(text, "%*s %s %s", guild_tag, char_name)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OPS_KICK_GUILD_MEMBER [guild tag][character name]", c_red3+127);
        return 0;
    }

    kick_guild_member(actor_node, guild_tag, char_name);
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


static int hash_list_maps(int actor_node, char *text) {

    /** RESULT  : lists available maps

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    (void)(text);

    int socket=clients.client[actor_node].socket;

    for(int i=0; i<MAX_MAPS; i++){

        if(strlen(maps.map[i].elm_filename)>0){

            send_text(socket, CHAT_SERVER, "%c%i %s", c_green3+127, i, maps.map[i].elm_filename);
        }
    }

    return 0;
}


static int hash_map(int actor_node, char *text) {

    /** RESULT  : extended map information

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char map_name[80]="";
    int map_id;

    if(sscanf(text, "%*s *s")==-1){

        //if no tail specified in command, default to current map
        map_id=clients.client[actor_node].map_id;
    }
    else if(sscanf(text, "%*s %[^\n]", map_name)==1){

        map_id=get_map_id(map_name);

        if(map_id==-1){

            send_text(socket, CHAT_SERVER, "%cMap does not exist", c_red3+127);
            return 0;
        }
    }
    else {

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #MAP [map_name] or #MAP", c_red3+127);
        return 0;
    }

    get_map_details(actor_node, map_id);
    get_map_developer_details(actor_node, map_id);

    return 0;
}


static int hash_track(int actor_node, char *text) {

    /** RESULT  : ops kicks a member from a a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    int socket=clients.client[actor_node].socket;
    char on_off[4]="";

    if(sscanf(text, "%*s %s", on_off)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #TRACK [ON / OFF]", c_red3+127);
        return 0;
    }

    str_conv_upper(on_off);

    if(strcmp(on_off, "ON")==0) {

        send_text(socket, CHAT_SERVER, "%cposition tracking 'on'", c_green3+127);
        clients.client[actor_node].track=true;
    }

    else if(strcmp(on_off, "OFF")==0) {

        send_text(socket, CHAT_SERVER, "%cposition tracking 'off'", c_green3+127);
        clients.client[actor_node].track=false;
    }

    else {

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #TRACK [ON / OFF]", c_red3+127);
        return 0;
    }

    return 0;
}


static int hash_trace(int actor_node, char *text) {

    /** RESULT  : trace walkable tiles

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    (void)(text);

    int socket=clients.client[actor_node].socket;
    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int pos_x=clients.client[actor_node].map_tile % map_axis;
    int pos_y=clients.client[actor_node].map_tile / map_axis;

    send_text(socket, CHAT_SERVER, "%cwalkable tile trace (@=%i/%i)", c_green3+127, pos_x, pos_y);

    for(int x=pos_x-20; x<pos_x+20; x++){

        char str[81]="";

        for(int y=pos_y-40; y<pos_y+40; y++){

            int z=(y * map_axis) + x;

            if(z==clients.client[actor_node].map_tile){

                printf("@");
                strcat(str, "@");
            }
            else {

                if(y>=0 && y<map_axis && x>=0 && x<map_axis){

                    if(maps.map[map_id].height_map[z]==NON_TRAVERSABLE_TILE){

                        printf("#");
                        strcat(str, "#");
                    }
                    else{

                        printf(".");
                        strcat(str, ".");
                    }
                }
                else{
                    printf(":");
                    strcat(str, ":");
                }
            }
        }

        send_text(socket, CHAT_SERVER, "%c%s", c_grey1+127, str);
        printf("\n");
    }

    return 0;
}


static int hash_trace_explore(int actor_node, char *text) {

    /** RESULT  : trace walkable tiles

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    (void)(text);

    int socket=clients.client[actor_node].socket;
    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int pos_x=clients.client[actor_node].map_tile % map_axis;
    int pos_y=clients.client[actor_node].map_tile / map_axis;

    send_text(socket, CHAT_SERVER, "%cexplorable tile trace (@=%i/%i)", c_green3+127, pos_x, pos_y);

    clients.client[actor_node].debug_explore_path=true;

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
    {"#GM",                      true,   0,     PERMISSION_1, hash_guild_message},
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
