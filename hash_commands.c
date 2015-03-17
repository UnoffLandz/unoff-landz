/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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
#include "db_character_tbl.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "gender.h"
#include "character_type.h"
#include "chat.h"
#include "game_data.h"

void process_hash_commands(int connection, char *text){

    /** public function - see header */

    char hash_command[80]="";
    char hash_command_tail[80]="";
    char text_out[1024]="";
    //char guild_tag[80]="";

    int command_parts=count_str_island(text);

    get_str_island(text, hash_command, 1);
    str_conv_upper(hash_command);

    log_event(EVENT_SESSION, "client [%i] character [%s] command [%s]", connection, clients.client[connection].char_name, hash_command);

/***************************************************************************************************/

    if(strcmp(hash_command, "#MOTD")==0){

        send_motd_file(connection);
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#BEAM_ME")==0 || strcmp(hash_command, "#BEAM")==0){

        //if hash command is #BEAM, check that second part is ME
        if(command_parts==2){

            char hash_command_second_part[80]="";

            get_str_island(text, hash_command_second_part, 2);
            str_conv_upper(hash_command_second_part);

            if(strcmp(hash_command_second_part, "ME")!=0) return;
        }

        //if char is moving when protocol arrives, cancel rest of path
        clients.client[connection].path_count=0;

        //ensure char doesn't beam on top of another char
        int new_map_tile=get_nearest_unoccupied_tile(game_data.beam_map_id, game_data.beam_map_tile);

        move_char_between_maps(connection, game_data.beam_map_id, new_map_tile);
    }

/***************************************************************************************************/
/*
    else if(strcmp(hash_command, "#NAME_CHANGE")==0){

        //check that #NAME_CHANGE command is properly formed (should have 2 parts delimited by an underline).
        if(command_parts!=2) {

            sprintf(text_out, "%cyou need to use the format #NAME_CHANGE [new name]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return;
        }

        get_str_island(text, hash_command_tail, 2);

        strcpy(old_char_name, clients.client[connection].char_name);

        if(rename_char(connection, hash_command_tail)==CHAR_RENAME_FAILED_DUPLICATE){

            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);

            log_event(EVENT_SESSION, "invalid name change attempt for char[%s] to [%s]", old_char_name, hash_command_tail);

            return;
        }

        sprintf(text_out, "%cIn the future you'll need to purchase a name change token to do this", c_yellow1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGratz. You just changed your character name to %s", c_green1+127, clients.client[connection].char_name);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        log_event(EVENT_SESSION, "name change for char[%s] to [%s]", old_char_name, clients.client[connection].char_name);
    }
*/
/***************************************************************************************************/

    else if(strcmp(hash_command, "#DETAILS")==0){

        //check that #DETAILS command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {

            sprintf(text_out, "%cyou need to use the format #DETAILS [character name]", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);
            return;
        }

        get_str_island(text, hash_command_tail, 2);

        //check that the char is in game
        int char_connection=char_in_game(hash_command_tail);

        if(char_connection==NOT_FOUND){

            sprintf(text_out, "%ccharacter is not in game", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);
            return;
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
/*
        sprintf(text_out, "%cGuild        :%s", c_green3+127, guilds.guild[character.guild_id]->guild_name);
        send_raw_text(connection, CHAT_SERVER, text_out);

        get_time_stamp_str(character.joined_guild, time_stamp_str);
        get_date_stamp_str(character.joined_guild, date_stamp_str);
        sprintf(text_out, "%cJoined       :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_raw_text(connection, CHAT_SERVER, text_out);
*/

        sprintf(text_out, "\n");
        send_raw_text(connection, CHAT_SERVER, text_out);
    }

/***************************************************************************************************/
/*
    else if(strcmp(hash_command, "#GM")==0){

        // check that #GM command is properly formed (should have 2 parts delimited by space)
        if(command_parts<2) {

            sprintf(text_out, "%cyou need to use the format #GM [message]", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);
            return;
        }

        //split the #GM command into separate elements
        get_str_island(text, hash_command_tail, 2);

        //send the message
        if(process_guild_chat(connection, message)==NOT_SENT){
            return;
        }
    }
*/
/***************************************************************************************************/
/*
    else if(strcmp(hash_command, "#IG")==0){

        //check that #IG command is properly formed (should have 3 parts delimited by spaces)
        if(command_parts<3) {

            sprintf(text_out, "%cyou need to use the format #IG [guild tag] [message]", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);
            return;
        }

        // split the #IG command into separate elements
        get_str_island(text, guild_tag, 2);
        get_str_island(text, message, 3);

        //send the message
        if(process_inter_guild_chat(connection, guild_tag, message)==NOT_SENT){
            return HASH_CMD_ABORTED;
        }
    }
*/
/***************************************************************************************************/

    else if(strcmp(hash_command, "#JC")==0 || strcmp(hash_command, "#JOIN_CHANNEL")==0){

        //check that #JC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {

            sprintf(text_out, "%cyou need to use the format #JC [channel number]", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            return;
        }

        // split the #JC command into channel number element
        get_str_island(text, hash_command_tail, 2);

        //convert channel number into an integer value
        int chan_id=atoi(hash_command_tail);

        //join the channel
        join_channel(connection, chan_id);
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#LC")==0 || strcmp(hash_command, "#LEAVE_CHANNEL")==0){

        //check that #LC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {

            sprintf(text_out, "%cyou need to use the format #LC [channel number]", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            return;
        }

        // split the #LC command into channel number element
        get_str_island(text, hash_command_tail, 2);

        //convert channel number into an integer value
        int chan_id=atoi(hash_command_tail);

        //leave the channel
        leave_channel(connection, chan_id);
    }
/***************************************************************************************************/

    else if (strcmp(hash_command, "#CL")==0 || strcmp(hash_command, "#CHANNEL_LIST")==0){

        sprintf(text_out, "\n%cNo   Channel    Description", c_blue1+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        int i=0;

        for(i=0; i<MAX_CHANNELS; i++){

            if(channel[i].chan_type!=CHAN_VACANT) {

                sprintf(text_out, "%c%i %s %-10s %-30s", c_blue1+127, i, "  ", channel[i].channel_name, channel[i].description);
                send_raw_text(connection, CHAT_SERVER, text_out);
            }
        }
    }
/***************************************************************************************************/

    else if (strcmp(hash_command, "#CP")==0 || strcmp(hash_command, "#CHANNEL_PARTICIPANTS")==0){

        int active_chan_slot=clients.client[connection].active_chan;

        printf("%i\n", active_chan_slot);

        if(active_chan_slot==0){

            sprintf(text_out, "%cNo active channel", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);
            return;
        }
        else {

            int chan_id=clients.client[connection].chan[active_chan_slot-31];

            list_characters_in_chan(connection, chan_id);
        }
    }
/***************************************************************************************************/

    else {

        sprintf(text_out, "%cCommand %s isn't supported. You may want to tell the game administrator", c_red3+127, text);
        send_raw_text(connection, CHAT_SERVER, text_out);

        log_event(EVENT_SESSION, "unknown #command [%s]", hash_command);
    }
 }
