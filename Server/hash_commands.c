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

static int hash_jc(int connection, char *text) {

    /** RESULT  : handles join channel hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    //get the chan number
    char chan_str[80]="";
    get_str_island(text, chan_str, 2);

    //convert channel number into an integer value
    int chan_id=atoi(chan_str);

    //join the channel
    join_channel(connection, chan_id);

    return 0;
}


static int hash_lc(int connection, char *text) {

    /** RESULT  : handles leave channel hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    //get the chan number
    char chan_str[80]="";
    get_str_island(text, chan_str, 2);

    //convert channel number into an integer value
    int chan_id=atoi(chan_str);

    //leave the channel
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


    char text_out[80]="";
    char char_name[80]="";
    get_str_island(text, char_name, 2);

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

    //if hash command is #BEAM, check that second part is ME
    if(count_str_island(text)==2){

        char part[80]="";

        get_str_island(text, part, 2);
        str_conv_upper(part);

        if(strcmp(part, "ME")!=0) return 0;
    }

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[connection].path_count=0;

    //ensure char doesn't beam on top of another char
    int new_map_tile=get_nearest_unoccupied_tile(game_data.beam_map_id, game_data.beam_map_tile);

    move_char_between_maps(connection, game_data.beam_map_id, new_map_tile);

    return 0;
}


static int hash_pm(int connection, char *text) {

    /** RESULT  : handles private message hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    //get character name
    char char_name[80]="";
    get_str_island(text, char_name, 2);

    //get message
    char msg[1024]="";
    get_str_island(text, msg, 3);

    //send pm
    send_pm(connection, char_name, msg);

    return 0;
}

static int hash_test(int connection, char *text) {

    /** RESULT  : test function

        RETURNS : void

        PURPOSE : debugging

        NOTES   :
    */

    char chan_str[80]="";
    get_str_island(text, chan_str, 2);

    //convert channel number into an integer value
    int chan_id=atoi(chan_str);

    clients.client[connection].active_chan=chan_id;
    printf("set active chan %i\n", chan_id);

    send_get_active_channels(connection);

    return 0;
}


typedef int (*hash_command_function)(int connection, char *text);

struct hash_command_array_entry {

    char command[80];
    int parts;
    hash_command_function fn;
    char warning[80];
};


struct hash_command_array_entry hash_command_entries[] = {

    {"#JC", 2, hash_jc, "you need to use the format #JC [channel number]"},
    {"#JOIN_CHANNEL", 2, hash_jc, "you need to use the format #JOIN_CHANNEL [channel number]"},
    {"#LC", 2, hash_lc, "you need to use the format #LC [channel number]"},
    {"#LEAVE_CHANNEL", 2, hash_lc, "you need to use the format #LEAVE_CHANNEL [channel number]"},
    {"#MOTD", 1, hash_motd, "you need to use the format #MOTD"},
    {"#MESSAGE_OF_THE_DAY", 1, hash_motd, "you need to use the format #MESSAGE_OF_THE_DAY"},
    {"#CL", 1, hash_cl, "you need to use the format #CL"},
    {"#CHANNEL_LIST", 1, hash_cl, "you need to use the format #CHANNEL_LIST"},
    {"#CP", 1, hash_cp, "you need to use the format #CP"},
    {"#CHANNEL_PARTICIPANTS", 1, hash_cp, "you need to use the format #CHANNEL_PARTICIPANTS"},
    {"#DETAILS", 2, hash_details, "you need to use the format #DETAILS [character name]"},
    {"#BEAM_ME", 1, hash_beam_me, "you need to use the format #BEAM_ME"},
    {"#BEAM", 2, hash_beam_me, "you need to use the format #BEAM ME"},
    {"#PM", 99, hash_pm, "you need to use the format #PM [message]"},
    {"#PRIVATE_MESSAGE", 99, hash_pm, "you need to use the format #PRIVATE_MESSAGE [message]"},
    {"#TEST", 2, hash_test, ""},
    { "", 0, 0, ""}
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

    //grab the first part of the text string as this should contain the command name
    char hash_command[80]="";
    int command_parts=count_str_island(text);
    get_str_island(text, hash_command, 1);
    str_conv_upper(hash_command);

    //lookup and execute the hash command function
    const struct hash_command_array_entry *hash_command_entry = find_hash_command_entry(hash_command);

    //if hash command doesn't exist then warn player and abort
    if(!hash_command_entry){

        sprintf(text_out, "%cCommand %s isn't supported. You may want to tell the game administrator", c_red3+127, text);
        send_raw_text(connection, CHAT_SERVER, text_out);

        log_event(EVENT_SESSION, "unknown #command [%s]", hash_command);

        return;
    }

    //test the hash command format and abort if incorrect
    //because pm, ig and gm messages may have multiple parts, we use hash command entry 99 which
    //specifies that actual parts must be greater than 1
    if(hash_command_entry->parts!=command_parts || (hash_command_entry->parts==99 && command_parts<2)){

        sprintf(text_out, "%c%s", c_red3+127, hash_command_entry->warning);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //execute the hash command
    hash_command_entry->fn(connection, text);

    log_event(EVENT_SESSION, "client [%i] character [%s] command [%s]", connection, clients.client[connection].char_name, hash_command);

    return;
}

