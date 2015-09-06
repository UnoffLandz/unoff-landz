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

static int hash_jc(int connection, char *text) {

    /** RESULT  : handles join channel hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    char command[80];
    int chan_id=0;

    if(sscanf(text, "%s %i", command, &chan_id)!=2){

        char text_out[80]="";
        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #JC [channel number]");
        send_raw_text(connection, CHAT_SERVER, text_out);

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

    char command[80];
    int chan_id=0;

    if(sscanf(text, "%s %i", command, &chan_id)!=2){

        char text_out[80]="";
        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #LC [channel number]");
        send_raw_text(connection, CHAT_SERVER, text_out);

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

    char text_out[80]="";

    sprintf(text_out, "\n%cNo   Channel    Description", c_blue1+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    for(int i=0; i<MAX_CHANNELS; i++){

        if(channel[i].chan_type!=CHAN_VACANT) {

            sprintf(text_out, "%c%i %s %-10s %-30s", c_blue1+127, i, "  ", channel[i].channel_name, channel[i].description);
            send_raw_text(connection, CHAT_SERVER, text_out);
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

    char text_out[80]="";

    int active_chan_slot=clients.client[connection].active_chan;

    if(active_chan_slot==0){

        sprintf(text_out, "%cNo active channel", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    int chan_id=clients.client[connection].chan[active_chan_slot-31];

    sprintf(text_out, "%cListing for channel [%i]: %s", c_blue1+127, chan_id, channel[chan_id].channel_name);
    send_raw_text(connection, CHAT_SERVER, text_out);

    sprintf(text_out, "%cDescription: %s", c_blue1+127, channel[chan_id].description);
    send_raw_text(connection, CHAT_SERVER, text_out);

    sprintf(text_out, "%cCharacters in channel...", c_blue1+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    for(int i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].client_status==LOGGED_IN){

            if(player_in_chan(i, chan_id)!=-1){

                sprintf(text_out, "%c%s ", c_blue1+127, clients.client[i].char_name);
                send_raw_text(connection, CHAT_SERVER, text_out);
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

    char command[80];
    char char_name[80]="";

    if(sscanf(text, "%s %s", command, char_name)!=2){

        send_text(connection, CHAT_SERVER, "%c%s", c_red3+127, "you need to use the format #DETAILS [character name]");

        return 0;
    }

    //check that the char is in game
    int char_connection=char_in_game(char_name);

    if(char_connection==NOT_FOUND){

        send_text(connection, CHAT_SERVER, "%c%s", c_red3+127, "character is not in game");

        return 0;
    }

    //send details to client
    send_text(connection, CHAT_SERVER, "%cCharacter    :%s", c_green3+127, clients.client[char_connection].char_name);

    int race_id=character_type[clients.client[char_connection].char_type].race_id;
    send_text(connection, CHAT_SERVER, "%cRace         :%s", c_green3+127, race[race_id].race_name);

    int gender_id=character_type[clients.client[char_connection].char_type].gender_id;
    send_text(connection, CHAT_SERVER, "%cGender       :%s", c_green3+127, gender[gender_id].gender_name);

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

    get_time_stamp_str(character.char_created, time_stamp_str);
    get_date_stamp_str(character.char_created, date_stamp_str);
    send_text(connection, CHAT_SERVER, "%cDate Created :%s %s", c_green3+127, date_stamp_str, time_stamp_str);

    send_text(connection, CHAT_SERVER, "%cCharacter Age:%i", c_green3+127, char_age(char_connection));

    if(character.guild_id==0){

        send_text(connection, CHAT_SERVER, "%cGuild        :guildless player", c_green3+127);
        send_text(connection, CHAT_SERVER, "%cJoined       :n/a", c_green3+127);
    }
    else {

        send_text(connection, CHAT_SERVER, "%cGuild        :%s", c_green3+127, guilds.guild[character.guild_id].guild_name);
        send_text(connection, CHAT_SERVER,"%cGuild tag    :%s", c_green3+127, guilds.guild[character.guild_id].guild_tag );

        get_time_stamp_str(character.joined_guild, time_stamp_str);
        get_date_stamp_str(character.joined_guild, date_stamp_str);
        send_text(connection, CHAT_SERVER, "%cJoined       :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
    }

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
    char text_out[80]="";

    if(sscanf(text, "%s %s", command, tail)==2){

        if(strcmp(tail, "ME")!=0){

            sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #BEAM_ME or #BEAM ME");
            send_raw_text(connection, CHAT_SERVER, text_out);

            return 0;
        }
    }

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[connection].path_count=0;

    //ensure char doesn't beam on top of another char
    int new_map_tile=get_nearest_unoccupied_tile(game_data.beam_map_id, game_data.beam_map_tile);

    //beam the char
    move_char_between_maps(connection, game_data.beam_map_id, new_map_tile);

    sprintf(text_out, "%c%s", c_green3+127, "Scotty beamed you up");
    send_raw_text(connection, CHAT_SERVER, text_out);

    return 0;
}


static int hash_pm(int connection, char *text) {

    /** RESULT  : handles private message hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    char command[80];
    char char_name[80];
    char message[1024];

    if(sscanf(text, "%s %s %s", command, char_name, message)!=3){

        char text_out[80]="";
        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #PM [character name][message]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    //send pm
    send_pm(connection, char_name, message);

    return 0;
}


static int hash_jump(int connection, char *text) {

    /** RESULT  : jumps char to a new map and map tile

        RETURNS : void

        PURPOSE : allows map developers to jump to unconnected maps

        NOTES   :
    */

    char command[80];
    int map_id=0;
    int map_tile=0;
    char text_out[80];

    if(sscanf(text, "%s %i %i", command, &map_id, &map_tile)!=3){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #JUMP [map id] [tile]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[connection].path_count=0;

    if(get_db_map_exists(map_id)==false){

        sprintf(text_out, "%cmap %i doesn't exist", c_red3+127, map_id);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    //ensure char doesn't beam on top of another char
    int new_map_tile=get_nearest_unoccupied_tile(map_id, map_tile);

    //send char to new map
    move_char_between_maps(connection, map_id, new_map_tile);

    sprintf(text_out, "%cYou jumped to map %s tile %i", c_green3+127, maps.map[map_id].map_name, new_map_tile);
    send_raw_text(connection, CHAT_SERVER, text_out);

    return 0;
}

static int hash_apply_guild_member(int connection, char *text) {

    /** RESULT  : application by char to join guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char command[80];
    char guild_tag[4];
    char text_out[80];

    if(sscanf(text, "%s %s", command, guild_tag)!=2){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #APPLY_GUILD [guild tag] or JG [guild tag]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    apply_guild(connection, clients.client[connection].char_name);

    return 0;
}


static int hash_ops_create_guild(int connection, char *text) {

    /** RESULT  : OPS command to create guild

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char command[80];
    char guild_name[80];
    char guild_tag[4];
    int permission_level=0;
    char text_out[80];

    if(sscanf(text, "%s %s %s %i", command, guild_name, guild_tag, &permission_level)!=3){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #OPS_CREATE_GUILD [guild name][guild tag][permission level]");
        send_raw_text(connection, CHAT_SERVER, text_out);

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

    char command[80];
    char guild_tag[4];
    char char_name[80];
    int guild_rank=0;
    char text_out[80];

    if(sscanf(text, "%s %s %s %i", command, guild_tag, char_name, &guild_rank)!=4){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #OPS_APPOINT_GUILD_MEMBER [guild tag][character_name][rank]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    join_guild(connection, guild_tag, char_name);

    return 0;
}


static int hash_change_guild_member_rank(int connection, char *text) {

    /** RESULT  : change a chars guild rank

        RETURNS : void

        PURPOSE :

        NOTES   : rank 18+ guild members only
    */

    char command[80];
    char char_name[80];
    int guild_rank=0;

    if(sscanf(text, "%s %s %i", command, char_name, &guild_rank)!=3){

        char text_out[80];

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #CHANGE_GUILD_RANK [character_name][rank] or #CR [character_name][rank]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    char *guild_tag=guilds.guild[clients.client[connection].guild_id].guild_tag;

    change_guild_rank(connection, char_name, guild_tag, guild_rank);

    return 0;
}


static int hash_ops_change_guild_member_rank(int connection, char *text) {

    /** RESULT  : change a chars guild rank

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char command[80]="";
    char char_name[80]="";
    char guild_tag[4]="";
    int guild_rank=0;

    if(sscanf(text, "%s %s %s %i", command, guild_tag, char_name, &guild_rank)!=4){

        char text_out[80];

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #OPS_CHANGE_GUILD_RANK [guild tag][character_name][rank]");
        send_raw_text(connection, CHAT_SERVER, text_out);

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

    char command[80]="";
    char guild_tag[4]="";
    int permission_level=0;

    if(sscanf(text, "%s %s %i", command, guild_tag, &permission_level)!=3){

        char text_out[80]="";

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #OPS_CHANGE_GUILD_PERMISSION [guild tag][permission level]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    change_guild_permission(connection, guild_tag, permission_level);

    return 0;
}


static int hash_list_guild_applicants(int connection, char *text) {

    /** RESULT  : change a guilds permission level

        RETURNS : void

        PURPOSE : OPS only

        NOTES   :
    */

    char command[80]="";
    char text_out[80]="";

    if(sscanf(text, "%s", command)!=1){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #LIST_GUILD_APPLICANTS or #LA");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    sprintf(text_out, "%cThe following characters have applied to join your guild...", c_green3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    int guild_id=clients.client[connection].guild_id;

    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp(guilds.guild[guild_id].applicant[i].char_name, "")!=0){

            char time_stamp_str[50]="";

            get_time_stamp_str(guilds.guild[guild_id].applicant[i].application_date, time_stamp_str);
            sprintf(text_out, "%c%s %s", c_green3+127, guilds.guild[guild_id].applicant[i].char_name, time_stamp_str);
            send_raw_text(connection, CHAT_SERVER, text_out);
       }
    }

    return 0;
}


static int hash_accept_guild_applicant(int connection, char *text) {

    /** RESULT  : accepts a chars application to join a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char command[80]="";
    char char_name[80]="";
    char text_out[80]="";

    if(sscanf(text, "%s %s", command, char_name)!=2){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #ACCEPT_GUILD_APPLICANTS [character name] or #AA [character name]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    join_guild(connection, char_name, guilds.guild[clients.client[connection].guild_id].guild_tag);

    sprintf(text_out, "%cYou have accepted the character's application to join your guild", c_green3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    int guild_id=clients.client[connection].guild_id;

    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp(char_name, guilds.guild[guild_id].applicant[i].char_name)==0){

            strcpy(guilds.guild[guild_id].applicant[i].char_name, "");
            guilds.guild[guild_id].applicant[i].application_date=0;
        }
    }

    return 0;
}


static int hash_reject_guild_applicant(int connection, char *text) {

    /** RESULT  : accepts a chars application to join a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char command[80]="";
    char char_name[80]="";
    char text_out[80]="";

    if(sscanf(text, "%s %s", command, char_name)!=2){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #REJECT_GUILD_APPLICANTS [character name] or #RA [character name]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    sprintf(text_out, "%cYou have rejected the character's application to join your guild", c_green3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    int guild_id=clients.client[connection].guild_id;

    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp(char_name, guilds.guild[guild_id].applicant[i].char_name)==0){

            strcpy(guilds.guild[guild_id].applicant[i].char_name, "");
            guilds.guild[guild_id].applicant[i].application_date=0;
        }
    }

    return 0;
}


static int hash_ops_kick_guild_member(int connection, char *text) {

    /** RESULT  : ops kicks a member from a a guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char command[80]="";
    char char_name[80]="";
    char guild_tag[4]="";
    char text_out[80]="";

    if(sscanf(text, "%s %s %s", command, guild_tag, char_name)!=3){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #OPS_KICK_GUILD_MEMBER [guild tag][character name]");
        send_raw_text(connection, CHAT_SERVER, text_out);

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

    char command[80]="";
    char char_name[80]="";
    char text_out[80]="";

    if(sscanf(text, "%s %s", command, char_name)!=2){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #KICK_GUILD_MEMBER [character name]");
        send_raw_text(connection, CHAT_SERVER, text_out);

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
    {"#AG",                     false, 0,       PERMISSION_1, hash_apply_guild_member},
    {"#APPLY_GUILD_MEMBER",     false, 0,       PERMISSION_1, hash_apply_guild_member},
    {"#OPS_CREATE_GUILD",       true,  0,       PERMISSION_3, hash_ops_create_guild},
    {"#OPS_APPOINT_GUILD_MEMBER", true,  0,     PERMISSION_3, hash_ops_appoint_guild_member},
    {"#CR",                     true,  18,      PERMISSION_1, hash_change_guild_member_rank},
    {"#CHANGE_GUILD_MEMBER_RANK", true,  18,    PERMISSION_1, hash_change_guild_member_rank},
    {"#OPS_CHANGE_GUILD_MEMBER_RANK", true, 0,  PERMISSION_3, hash_ops_change_guild_member_rank},
    {"#OPS_CHANGE_GUILD_PERMISSION",true, 20,   PERMISSION_3, hash_ops_change_guild_permission},
    {"#LA",                      true,   18,    PERMISSION_1, hash_list_guild_applicants},
    {"#LIST_GUILD_APPLICANTS",  true,   18,     PERMISSION_1, hash_list_guild_applicants},
    {"#AA",                     true,   18,     PERMISSION_1, hash_accept_guild_applicant},
    {"#ACCEPT_GUILD_APPLICANT",  true,   18,    PERMISSION_1, hash_accept_guild_applicant},
    {"#RA",                      true,   18,    PERMISSION_1, hash_reject_guild_applicant},
    {"#REJECT_GUILD_APPLICANT",  true,   18,    PERMISSION_1, hash_reject_guild_applicant},
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

    char text_out[80]="";

    //grab the first part of the text string as this contains the hash command name
    char hash_command[80]="";
    sscanf(text, "%s", hash_command);
    str_conv_upper(hash_command);

    //lookup the hash command function
    const struct hash_command_array_entry *hash_command_entry = find_hash_command_entry(hash_command);

    //check if hash command exists
    if(!hash_command_entry){

        sprintf(text_out, "%ccommand %s is not supported", c_red3+127, hash_command);
        send_raw_text(connection, CHAT_SERVER, text_out);

        log_event(EVENT_SESSION, "unknown #command [%s]", hash_command);
        return;
    }

    //prevent guildless players from using guild commands
    int guild_id=clients.client[connection].guild_id;

    if(hash_command_entry->guild_command==true && guild_id==0){

        sprintf(text_out, "%cCommand can only be used by guild members", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);
        return;
    }

    //prevent guild players for using commands above their rank
    int required_rank=hash_command_entry->guild_rank;

    if(guild_id>0 && required_rank>clients.client[connection].guild_rank){

        sprintf(text_out, "%cCommand can only be used by guild members with rank %i or above", c_red3+127, required_rank);
        send_raw_text(connection, CHAT_SERVER, text_out);
        return;
    }

    //prevent guilds from using commands above their permission
    int permission_level=guilds.guild[guild_id].permission_level;

    if(guild_id>0 && hash_command_entry->permission_level>permission_level){

        sprintf(text_out, "%cYou are not authorised to use that command", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);
        return;
    }

    //execute the hash command
    hash_command_entry->fn(connection, text);

    log_event(EVENT_SESSION, "client [%i] character [%s] command [%s]", connection, clients.client[connection].char_name, text);
}

