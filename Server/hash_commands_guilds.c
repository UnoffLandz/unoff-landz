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
#include "movement.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "gender.h"
#include "character_type.h"
#include "chat.h"
#include "game_data.h"
#include "guilds.h"
#include "idle_buffer2.h"
#include "broadcast_actor_functions.h"

int hash_set_guild_tag_colour(int connection, char *text) {

    /** public function - see header */

    char guild_tag_colour[80]="";

    if(sscanf(text, "%*s %s", guild_tag_colour)!=1){

        send_text(connection, CHAT_SERVER,"you need to use the format #SET_GUILD_COLOUR [colour] or #SC [colour]", c_red3+127);
        return 0;
    }

    int colour_code=get_colour_code(guild_tag_colour);

    if(colour_code==-1){

        send_text(connection, CHAT_SERVER,"not a valid colour", c_red3+127);
        return 0;
    }

    int guild_id=clients.client[connection].guild_id;
    guilds.guild[guild_id].guild_tag_colour=colour_code;

    //update database
    push_sql_command("UPDATE GUILD_TABLE SET GUILD_TAG_COLOUR=%i WHERE GUILD_ID=%i", guilds.guild[guild_id].guild_tag_colour);

    send_text(connection, CHAT_SERVER, "%cyou have changed the guild tag_colour to %c%s", c_green3+127, guilds.guild[guild_id].guild_tag_colour, guilds.guild[guild_id].guild_tag);

    return 0;
}


int hash_set_guild_description(int connection, char *text) {

    /** public function - see header */

    char description[160]="";

    if(sscanf(text, "%*s %s", description)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #SET_GUILD_DESCRIPTION [description] or #SD [description]", c_red3+127);

        return 0;
    }

    int guild_id=clients.client[connection].guild_id;

    strcpy(guilds.guild[guild_id].guild_description, description);

    //update database
    push_sql_command("UPDATE GUILD_TABLE SET DESCRIPTION='%s' WHERE GUILD_ID=%i", guilds.guild[guild_id].guild_description, guild_id);

    send_text(connection, CHAT_SERVER, "%cyou have changed the guild description", c_green3+127);

    return 0;
}


int hash_guild_message(int connection, char *text) {

    /** public function - see header */

    char message[1024]="";
    int guild_id=clients.client[connection].guild_id;

    //scans the string from the second element onwards
    if(sscanf(text, "%*s %[^\n]", message)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #GUILD_MESSAGE [message] or #GM [message] or ~[message]", c_red3+127);
        return 0;
    }

    //broadcast to self
    send_text(connection, CHAT_GM, "%c[%s]: %s", c_blue1+127, clients.client[connection].char_name, message);

    //broadcast to others
    broadcast_guild_chat(guild_id, connection, message);
    return 0;
}


int hash_guild_details(int connection, char *text) {

    /** public function - see header */

    char guild_tag[80]="";
    int guild_id;

    if(sscanf(text, "%*s %s", guild_tag)==-1){

        // if no tail specified in command, default to current guild
        guild_id=clients.client[connection].guild_id;
    }
    else if(sscanf(text, "%*s %s", guild_tag)==1){

        // if tail specified in command, use that to indicate guild
        guild_id=get_guild_id(guild_tag);
        if(guild_id==-1){

            send_text(connection, CHAT_SERVER, "%cGuild does not exist", c_red3+127);
            return 0;
        }
    }
    else {

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #GUILD_DETAILS [guild_tag] or #GD [guild_tag]", c_red3+127);
        return 0;
    }

    push_command(connection, IDLE_BUFFER_PROCESS_GUILD_DETAILS , "", guild_id);

    return 0;
}


int hash_list_guild(int connection, char *text) {

    /** public function - see header */

    char list_type[40]="";

    // if list type is not specified, default to RANK
    if(sscanf(text, "%*s %s", list_type)==-1){

        push_command(connection, IDLE_BUFFER_PROCESS_LIST_GUILD_BY_RANK, "", 0);
        return 0;
    }

    // if list type is specified use that type
    if(sscanf(text, "%*s %s", list_type)==1){

        str_conv_upper(list_type);

        if(strcmp(list_type, "R")==0 || strcmp(list_type, "RANK")==0){

            push_command(connection, IDLE_BUFFER_PROCESS_LIST_GUILD_BY_RANK, "", 0);
            return 0;
        }
        else if(strcmp(list_type, "T")==0 || strcmp(list_type, "TIME")==0){

            push_command(connection, IDLE_BUFFER_PROCESS_LIST_GUILD_BY_TIME, "", 0);
            return 0;
        }
    }

    send_text(connection, CHAT_SERVER, "%cyou need to use the format #LIST_GUILD [RANK/TIME] or #LG [R/T]", c_red3+127);
    return 0;
}


int hash_kick_guild_member(int connection, char *text) {

    /** public function - see header */

    char char_name[80]="";

    if(sscanf(text, "%*s %s", char_name)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #KICK_GUILD_MEMBER [character name]", c_red3+127);

        return 0;
    }

    kick_guild_member(connection, guilds.guild[clients.client[connection].guild_id].guild_tag, char_name);
    return 0;
}


int hash_reject_applicant(int connection, char *text) {

    /** public function - see header */

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


int hash_accept_applicant(int connection, char *text) {

    /** public function - see header */

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


int hash_list_applicants(int connection, char *text) {

    /** public function - see header */

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


int hash_change_rank(int connection, char *text) {

    /** public function - see header */

    char char_name[80];
    int guild_rank=0;

    if(sscanf(text, "%*s %s %i", char_name, &guild_rank)!=2){

        send_text(connection, CHAT_SERVER,"you need to use the format #CHANGE_RANK [character_name][rank] or #CR [character_name][rank]", c_red3+127);
        return 0;
    }

    change_guild_rank(connection, char_name, guilds.guild[clients.client[connection].guild_id].guild_tag, guild_rank);
    return 0;
}


int hash_apply_guild(int connection, char *text) {


    char guild_tag[GUILD_TAG_LENGTH];

    if(sscanf(text, "%*s %s", guild_tag)!=1){

        send_text(connection, CHAT_SERVER, "%cyou need to use the format #APPLY_GUILD [guild tag] or #AG [guild tag]", c_red3+127);
        return 0;
    }

    apply_guild(connection, guild_tag);
    return 0;
}


int hash_leave_guild(int connection, char *text) {

    /** public function - see header */

    (void)(text);

    int guild_id=clients.client[connection].guild_id;

    //update database
    push_sql_command("UPDATE CHARACTER_TABLE SET GUILD_ID=0, GUILD_RANK=0, DATE_JOINED_GUILD=0 WHERE GUILD_ID=%i", guilds.guild[guild_id].guild_description, guild_id);

    send_text(connection, CHAT_SERVER, "%cyou have left %s guild", c_green3+127, guilds.guild[guild_id].guild_tag);

    return 0;
}
