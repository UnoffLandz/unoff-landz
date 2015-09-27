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
#include "hash_commands.h"
#include "colour.h"
#include "server_messaging.h"
#include "maps.h"
#include "global.h"
#include "movement.h"
#include "db/db_character_tbl.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "gender.h"
#include "character_type.h"
#include "chat.h"
#include "game_data.h"
#include "guilds.h"
#include "db/db_map_tbl.h"
#include "idle_buffer2.h"

static int hash_jc(int connection, char *text) {

    /** RESULT  : handles join channel hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    int chan_id=0;

    if(sscanf(text, "%*s %i", &chan_id)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #JC [channel number]", c_red3+127);
        return 0;
    }

    join_channel(connection, chan_id);

    return 0;
}


static int hash_lc(int connection, char *text) {

    /** RESULT  : handles leave channel hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    int chan_id=0;

    if(sscanf(text, "%*s %i", &chan_id)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #LC [channel number]", c_red3+127);
        return 0;
    }

    leave_channel(connection, chan_id);

    return 0;
}


static int hash_motd(int connection, char *text) {

    /** RESULT  : handles motd hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    (void)(text);

    send_motd_file(connection);

    return 0;
}


static int hash_cl(int connection, char *text) {

    /** RESULT  : handles channel list hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    (void)(text);

    send_text(connection, CHAT_SERVER, "\n%cNo   Channel    Description", c_blue1+127);

    for(int i=0; i<MAX_CHANNELS; i++){

        if(channel[i].chan_type!=CHAN_VACANT) {

            send_text(connection, CHAT_SERVER, "%c%i %s %-10s %-30s", c_blue1+127, i, "  ", channel[i].channel_name, channel[i].description);
        }
    }

    return 0;
}


static int hash_cp(int connection, char *text) {

    /** RESULT  : handles channel participants hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    (void)(text);

    int active_chan_slot=clients.client[connection].active_chan;

    if(active_chan_slot==0){

        send_text(connection, CHAT_SERVER, "%cNo active channel", c_red3+127);
        return 0;
    }

    int chan_id=clients.client[connection].chan[active_chan_slot-31];

    send_text(connection, CHAT_SERVER, "%cListing for channel [%i]: %s", c_blue1+127, chan_id, channel[chan_id].channel_name);
    send_text(connection, CHAT_SERVER, "%cDescription: %s", c_blue1+127, channel[chan_id].description);
    send_text(connection, CHAT_SERVER, "%cCharacters in channel...", c_blue1+127);

    for(int i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].client_status==LOGGED_IN){

            if(player_in_chan(i, chan_id)!=-1){

                send_text(connection, CHAT_SERVER, "%c%s ", c_blue1+127, clients.client[i].char_name);
            }
        }
    }

    return 0;
}


static int hash_details(int connection, char *text) {

    /** RESULT  : handles details hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    char char_name[80]="";

    // if char name not specified, substitute with calling char name
    if(sscanf(text, "%*s %s", char_name)==-1){

        push_command(connection, IDLE_BUFFER_PROCESS_HASH_DETAILS, clients.client[connection].char_name);
        return 0;
    }

    // if char name is specified use that name
    if(sscanf(text, "%*s %s", char_name)==1){

        push_command(connection, IDLE_BUFFER_PROCESS_HASH_DETAILS, char_name);
        return 0;
    }

    send_text(connection, CHAT_SERVER, "%c%s", c_red3+127, "you need to use the format #DETAILS [character name]");

    return 0;
}


static int hash_beam_me(int connection, char *text) {

    /** RESULT  : handles #beam_me hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    char command[80];
    char tail[80];

    if(sscanf(text, "%s %s", command, tail)>2){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #BEAM_ME or #BEAM ME", c_red3+127);
        return 0;
    }

    str_conv_upper(command);
    str_conv_upper(tail);

    if(strcmp(command, "BEAM_ME")!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #BEAM_ME or #BEAM ME", c_red3+127);
        return 0;
    }

    if(strcmp(command, "BEAM")!=0 || strcmp(tail, "ME")!=0){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #BEAM_ME or #BEAM ME", c_red3+127);
        return 0;
    }

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[connection].path_count=0;

    //ensure char doesn't beam on top of another char
    int new_map_tile=get_nearest_unoccupied_tile(game_data.beam_map_id, game_data.beam_map_tile);

    //beam the char
    move_char_between_maps(connection, game_data.beam_map_id, new_map_tile);

    send_text(connection, CHAT_SERVER, "%cScotty beamed you up", c_green3+127);

    return 0;
}


static int hash_pm(int connection, char *text) {

    /** RESULT  : handles private message hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    char char_name[80];
    char message[1024];

    if(sscanf(text, "%*s %s %s", char_name, message)!=2){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #PM [character name][message]", c_red3+127);
        return 0;
    }

    send_pm(connection, char_name, message);

    return 0;
}


static int hash_jump(int connection, char *text) {

    /** RESULT  : jumps char to a new map and map tile

        RETURNS : void

        PURPOSE : allows map developers to jump to unconnected maps

        NOTES   :
    */

    int map_id=0;
    int map_tile=0;

    if(sscanf(text, "%*s %i %i", &map_id, &map_tile)!=2){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #JUMP [map id] [tile]", c_red3+127);
        return 0;
    }

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[connection].path_count=0;

    if(get_db_map_exists(map_id)==false){

        send_text(connection, CHAT_SERVER, "%cmap %i doesn't exist", c_red3+127, map_id);
        return 0;
    }

    //ensure char doesn't beam on top of another char
    int new_map_tile=get_nearest_unoccupied_tile(map_id, map_tile);

    //send char to new map
    move_char_between_maps(connection, map_id, new_map_tile);

    send_text(connection, CHAT_SERVER, "%cYou jumped to map %s tile %i", c_green3+127, maps.map[map_id].map_name, new_map_tile);

    return 0;
}


static int hash_apply_guild(int connection, char *text) {

    /** RESULT  : application by char to join guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char guild_tag[GUILD_TAG_LENGTH];

    if(sscanf(text, "%*s %s", guild_tag)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #APPLY_GUILD [guild tag] or #AG [guild tag]", c_red3+127);
        return 0;
    }

    apply_guild(connection, guild_tag);

    return 0;
}


static int hash_ops_create_guild(int connection, char *text) {

    /** RESULT  : OPS command to create guild

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char guild_name[80];
    char guild_tag[GUILD_TAG_LENGTH];
    int permission_level=0;

    if(sscanf(text, "%*s %s %s %i", guild_name, guild_tag, &permission_level)!=3){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #OPS_CREATE_GUILD [guild name][guild tag][permission level]", c_red3+127);
        return 0;
    }

    create_guild(connection, guild_name, "", guild_tag, permission_level);

    return 0;
}


static int hash_ops_appoint_guild_member(int connection, char *text) {

    /** RESULT  : OPS command to appoint a player to a guild

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char char_name[80]="";
    char guild_tag[GUILD_TAG_LENGTH]="";

    if(sscanf(text, "%*s %s %s", char_name, guild_tag)!=2){

        send_text(connection, CHAT_SERVER, "you need to use the format #OPS_APPOINT_GUILD_MEMBER [character name][guild tag]", c_red3+127);
        return 0;
    }

    join_guild(connection, char_name, guild_tag);

    return 0;
}


static int hash_change_rank(int connection, char *text) {

    /** RESULT  : change a chars guild rank

        RETURNS : void

        PURPOSE :

        NOTES   : rank 18+ guild members only
    */

    char char_name[80];
    int guild_rank=0;

    if(sscanf(text, "%*s %s %i", char_name, &guild_rank)!=2){

        send_text(connection, CHAT_SERVER,"you need to use the format #CHANGE_RANK [character_name][rank] or #CR [character_name][rank]", c_red3+127);
        return 0;
    }

    change_guild_rank(connection, char_name, guilds.guild[clients.client[connection].guild_id].guild_tag, guild_rank);

    return 0;
}


static int hash_ops_change_guild_member_rank(int connection, char *text) {

    /** RESULT  : change a chars guild rank

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char char_name[80]="";
    char guild_tag[GUILD_TAG_LENGTH]="";
    int guild_rank=0;

    if(sscanf(text, "%*s %s %s %i", char_name, guild_tag, &guild_rank)!=3){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #OPS_CHANGE_GUILD_MEMBER_RANK [character_name][guild_tag][rank]", c_red3+127);
        return 0;
    }

    change_guild_rank(connection, char_name, guild_tag, guild_rank);

    return 0;
}


static int hash_ops_change_guild_permission(int connection, char *text) {

    /** RESULT  : change a guilds permission level

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char guild_tag[GUILD_TAG_LENGTH]="";
    int permission_level=0;

    if(sscanf(text, "%*s %s %i", guild_tag, &permission_level)!=2){

        send_text(connection, CHAT_SERVER, "you need to use the format #OPS_CHANGE_GUILD_PERMISSION [guild tag][permission level]", c_red3+127);
        return 0;
    }

    change_guild_permission(connection, guild_tag, permission_level);

    return 0;
}


static int hash_list_applicants(int connection, char *text) {

    /** RESULT  : change a guilds permission level

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    (void)(text);

    send_text(connection, CHAT_SERVER,"%cThe following characters have applied to join your guild...", c_green3+127 );

    int guild_id=clients.client[connection].guild_id;

    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp(guilds.guild[guild_id].applicant[i].char_name, "")!=0){

            char date_stamp_str[50]="";
            get_date_stamp_str(guilds.guild[guild_id].applicant[i].application_date, date_stamp_str);

            char time_stamp_str[50]="";
            get_time_stamp_str(guilds.guild[guild_id].applicant[i].application_date, time_stamp_str);

            send_text(connection, CHAT_SERVER, "%c%s %s %s", c_green3+127, guilds.guild[guild_id].applicant[i].char_name, date_stamp_str, time_stamp_str);
       }
    }

    return 0;
}


static int hash_accept_applicant(int connection, char *text) {

    /** RESULT  : accepts a chars application to join a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char char_name[80]="";

    if(sscanf(text, "%*s %s", char_name)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #ACCEPT_APPLICANT [character name] or #AA [character name]", c_red3+127);
        return 0;
    }

    join_guild(connection, char_name, guilds.guild[clients.client[connection].guild_id].guild_tag);

    //update the guild applicants list
    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp_upper(char_name, guilds.guild[clients.client[connection].guild_id].applicant[i].char_name)==0){

            strcpy(guilds.guild[clients.client[connection].guild_id].applicant[i].char_name, "");
            guilds.guild[clients.client[connection].guild_id].applicant[i].application_date=0;
            return 0;
        }
    }

    return 0;
}


static int hash_reject_applicant(int connection, char *text) {

    /** RESULT  : accepts a chars application to join a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char char_name[80]="";

    if(sscanf(text, "%*s %s", char_name)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #REJECT_APPLICANT [character name] or #RA [character name]", c_red3+127);
        return 0;
    }

    //update the guild applicants list
    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp_upper(char_name, guilds.guild[clients.client[connection].guild_id].applicant[i].char_name)==0){

            strcpy(guilds.guild[clients.client[connection].guild_id].applicant[i].char_name, "");
            guilds.guild[clients.client[connection].guild_id].applicant[i].application_date=0;
            send_text(connection, CHAT_SERVER, "%cyou rejected the guild application from %s", c_green3+127, char_name);
            return 0;
        }
    }

    send_text(connection, CHAT_SERVER, "can't find application to join guild from %s", c_red3+127, char_name);
    return 0;
}


static int hash_ops_kick_guild_member(int connection, char *text) {

    /** RESULT  : ops kicks a member from a a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char char_name[80]="";
    char guild_tag[GUILD_TAG_LENGTH]="";

    if(sscanf(text, "%*s %s %s", guild_tag, char_name)!=2){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #OPS_KICK_GUILD_MEMBER [guild tag][character name]", c_red3+127);
        return 0;
    }

    kick_guild_member(connection, guild_tag, char_name);

    return 0;
}


static int hash_kick_guild_member(int connection, char *text) {

    /** RESULT  : ops kicks a member from a a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char char_name[80]="";

    if(sscanf(text, "%*s %s", char_name)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #KICK_GUILD_MEMBER [character name]", c_red3+127);

        return 0;
    }

    kick_guild_member(connection, guilds.guild[clients.client[connection].guild_id].guild_tag, char_name);

    return 0;
}


typedef int (*hash_command_function)(int connection, char *text);


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
    {"#DETAILS",                false, 0,       PERMISSION_1, hash_details},
    {"#BEAM_ME",                false, 0,       PERMISSION_1, hash_beam_me},
    {"#BEAM",                   false, 0,       PERMISSION_1, hash_beam_me},
    {"#PM",                     false, 0,       PERMISSION_1, hash_pm},
    {"#PRIVATE_MESSAGE",        false, 0,       PERMISSION_1, hash_pm},
    {"#JUMP",                   false, 0,       PERMISSION_2, hash_jump},
    {"#AG",                     false, 0,       PERMISSION_1, hash_apply_guild},
    {"#APPLY_GUILD",            false, 0,       PERMISSION_1, hash_apply_guild},
    {"#OPS_CREATE_GUILD",       true,  0,       PERMISSION_3, hash_ops_create_guild},
    {"#OPS_APPOINT_GUILD_MEMBER", true,  0,     PERMISSION_3, hash_ops_appoint_guild_member},
    {"#CR",                      true,  18,     PERMISSION_1, hash_change_rank},
    {"#CHANGE_RANK",             true,  18,     PERMISSION_1, hash_change_rank},
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


void process_hash_commands(int connection, char *text){

    /** public function - see header */

    //grab the first part of the text string as this contains the hash command name
    char hash_command[80]="";
    sscanf(text, "%s", hash_command);
    str_conv_upper(hash_command);

    //lookup the hash command function
    const struct hash_command_array_entry *hash_command_entry = find_hash_command_entry(hash_command);

    //check if hash command exists
    if(!hash_command_entry){

        send_text(connection, CHAT_SERVER, "%ccommand %s is not supported", c_red3+127, hash_command);
        log_event(EVENT_SESSION, "unknown #command [%s]", hash_command);
        return;
    }

    //prevent guildless players from using guild commands
    int guild_id=clients.client[connection].guild_id;

    if(hash_command_entry->guild_command==true && guild_id==0){

        send_text(connection, CHAT_SERVER, "%cCommand can only be used by guild members", c_red3+127 );
        return;
    }

    //prevent guild players for using commands above their rank
    int required_rank=hash_command_entry->guild_rank;

    if(guild_id>0 && required_rank>clients.client[connection].guild_rank){

        send_text(connection, CHAT_SERVER, "%cCommand can only be used by guild members with rank %i or above", c_red3+127, required_rank);
        return;
    }

    //prevent guilds from using commands above their permission
    int permission_level=guilds.guild[guild_id].permission_level;

    if(guild_id>0 && hash_command_entry->permission_level>permission_level){

        send_text(connection, CHAT_SERVER, "%cYou are not authorised to use that command", c_red3+127);
        return;
    }

    //execute the hash command
    hash_command_entry->fn(connection, text);

    log_event(EVENT_SESSION, "client [%i] character [%s] command [%s]", connection, clients.client[connection].char_name, text);
}

