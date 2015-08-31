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
    char text_out[80]="";

    if(sscanf(text, "%s %s", command, char_name)!=2){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #DETAILS [character name]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    //check that the char is in game
    int char_connection=char_in_game(char_name);

    if(char_connection==NOT_FOUND){

        sprintf(text_out, "%ccharacter is not in game", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    //send details to client
    sprintf(text_out, "%cCharacter    :%s", c_green3+127, clients.client[char_connection].char_name);
    send_raw_text(connection, CHAT_SERVER, text_out);

    int race_id=character_type[clients.client[char_connection].char_type].race_id;
    sprintf(text_out, "%cRace         :%s", c_green3+127, race[race_id].race_name);
    send_raw_text(connection, CHAT_SERVER, text_out);

    int gender_id=character_type[clients.client[char_connection].char_type].gender_id;
    sprintf(text_out, "%cGender       :%s", c_green3+127, gender[gender_id].gender_name);
    send_raw_text(connection, CHAT_SERVER, text_out);

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

    get_time_stamp_str(character.char_created, time_stamp_str);
    get_date_stamp_str(character.char_created, date_stamp_str);
    sprintf(text_out, "%cDate Created :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
    send_raw_text(connection, CHAT_SERVER, text_out);

    sprintf(text_out, "%cCharacter Age:%i", c_green3+127, char_age(char_connection));
    send_raw_text(connection, CHAT_SERVER, text_out);

    if(character.guild_id==0){

        sprintf(text_out, "%cGuild        :guildless player", c_green3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cJoined       :n/a", c_green3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);
    }
    else {

        sprintf(text_out, "%cGuild        :%s", c_green3+127, guilds.guild[character.guild_id].guild_name);
        send_raw_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGuild tag    :%s", c_green3+127, guilds.guild[character.guild_id].guild_tag);
        send_raw_text(connection, CHAT_SERVER, text_out);

        get_time_stamp_str(character.joined_guild, time_stamp_str);
        get_date_stamp_str(character.joined_guild, date_stamp_str);

        sprintf(text_out, "%cJoined       :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_raw_text(connection, CHAT_SERVER, text_out);
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

static int hash_apply_guild(int connection, char *text) {

    /** RESULT  : application by char to join guild

        RETURNS : void

        PURPOSE :

        NOTES   :
    */

    char command[80];
    char guild_tag[4];
    char text_out[80];

    if(sscanf(text, "%s %s", command, guild_tag)!=2){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #JOIN_GUILD [guild tag] or JG [guild tag]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    apply_guild(connection, clients.client[connection].char_name);

    return 0;
}


static int hash_create_guild(int connection, char *text) {

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

    if(sscanf(text, "%s %s %s %i", command, guild_name, guild_tag, &permission_level)!=4){

        sprintf(text_out, "%c%s", c_red3+127, "you need to use the format #CREATE_GUILD [guild name][guild tag][permission level or CG [guild name][guild tag][permission level]");
        send_raw_text(connection, CHAT_SERVER, text_out);

        return 0;
    }

    create_guild(connection, guild_name, guild_tag, permission_level);

    return 0;
}

typedef int (*hash_command_function)(int connection, char *text);

struct hash_command_array_entry {

    char command[80];
    hash_command_function fn;
};


struct hash_command_array_entry hash_command_entries[] = {

    {"#JC", hash_jc},
    {"#JOIN_CHANNEL", hash_jc},
    {"#LC", hash_lc},
    {"#LEAVE_CHANNEL", hash_lc},
    {"#MOTD", hash_motd},
    {"#MESSAGE_OF_THE_DAY", hash_motd},
    {"#CL", hash_cl},
    {"#CHANNEL_LIST", hash_cl},
    {"#CP", hash_cp},
    {"#CHANNEL_PARTICIPANTS", hash_cp},
    {"#DETAILS", hash_details},
    {"#BEAM_ME", hash_beam_me},
    {"#BEAM", hash_beam_me},
    {"#PM", hash_pm},
    {"#PRIVATE_MESSAGE", hash_pm},
    {"#JUMP", hash_jump},
    {"#AG", hash_apply_guild},
    {"#APPLY_GUILD", hash_apply_guild},
    {"#CG", hash_create_guild},
    {"#CREATE_GUILD", hash_create_guild},
    { "", 0}
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

    //grab the first part of the text string as this should contain the hash command name
    char hash_command[80]="";
    sscanf(text, "%s", hash_command);
    str_conv_upper(hash_command);

    //lookup the hash command function
    const struct hash_command_array_entry *hash_command_entry = find_hash_command_entry(hash_command);

    //check if hash command exists
    if(!hash_command_entry){

        sprintf(text_out, "%cCommand %s isn't supported. You may want to tell the game administrator", c_red3+127, text);
        send_raw_text(connection, CHAT_SERVER, text_out);

        log_event(EVENT_SESSION, "unknown #command [%s]", hash_command);

        return;
    }

    //execute the hash command
    hash_command_entry->fn(connection, text);

    log_event(EVENT_SESSION, "client [%i] character [%s] command [%s]", connection, clients.client[connection].char_name, hash_command);
}

