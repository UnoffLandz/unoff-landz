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

#include <stdio.h> //support sprintf
#include <string.h> //support strcpy and strcmp

#include "guilds.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "server_messaging.h"
#include "clients.h"
#include "logging.h"
#include "db/database_functions.h"
#include "db/db_character_tbl.h"
#include "db/db_guild_tbl.h"
#include "server_start_stop.h"
#include "idle_buffer2.h"
#include "characters.h"
#include "string_functions.h"
#include "date_time_functions.h"

struct guild_list_type guilds;
struct guild_member_list_type guild_member_list;

void update_guild_details(int character_id, int guild_id, time_t joined_guild, int guild_rank){

    /** RESULT   :

        RETURNS  : void

        PURPOSE  : used by function hash_change_guild_permission

        NOTES    : used by functions kick_guild_member
                                     change_guild_rank
                                     join_guild
    **/

    //update database
    push_sql_command("UPDATE CHARACTER_TABLE SET GUILD_ID=%i, JOINED_GUILD=%i, GUILD_RANK=%i WHERE CHAR_ID=%i", guild_id, (int)joined_guild, guild_rank, character_id);

    //if char in game then update client data
    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].character_id==character_id){

            clients.client[i].guild_id=guild_id;
            clients.client[i].joined_guild=joined_guild;
            clients.client[i].guild_rank=guild_rank;

            //re-add char to map so that guild tag changes are immediately visible
            if(add_char_to_map(i, clients.client[i].map_id, clients.client[i].map_tile)==false){

               log_event(EVENT_ERROR, "unable to add char[%s] to map in function %s: module %s: line %i", clients.client[i].char_name, __func__, __FILE__, __LINE__);
               stop_server();
            }

            return;
        }
    }
}


int get_guild_id(char *guild_tag){

    /** public function - see header **/

    for(int i=0; i<MAX_GUILDS; i++){

        if(strcmp_upper(guild_tag, guilds.guild[i].guild_tag)==0){

            return i;
        }
    }

    return -1;
}


void apply_guild(int actor_node, char *guild_tag){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    //check that applicant is guildless (and therefore eligible tp join a guild)
    int guild_id=clients.client[actor_node].guild_id;

    if(guild_id!=0){

        send_text(socket, CHAT_SERVER, "%cyou are already in guild %s", c_red3+127, guilds.guild[guild_id].guild_name);
        return;
    }

    //check that guild exists
    guild_id=get_guild_id(guild_tag);

    if(guild_id==-1){

        send_text(socket, CHAT_SERVER, "%cguild %s does not exist", c_red3+127, guild_tag);
        return;
    }

    //check for duplicate entry in guild applicants list
    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp_upper(guilds.guild[guild_id].applicant[i].char_name, clients.client[actor_node].char_name)==0){

            send_text(socket, CHAT_SERVER, "%cyou have already applied to join guild %s", c_red3+127, guild_tag);
            return;
        }
    }

    //add new entry to guild applicant list
    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp(guilds.guild[guild_id].applicant[i].char_name, "")==0){

            strcpy(guilds.guild[guild_id].applicant[i].char_name, clients.client[actor_node].char_name);
            guilds.guild[guild_id].applicant[i].application_date=time(NULL);
            send_text(socket, CHAT_SERVER, "%cyou applied to join guild %s", c_green3+127, guild_tag);
            return;
        }
    }

    send_text(socket, CHAT_SERVER, "%cguild %s has too many pending applications. Please contact the guild master", c_red3+127, guild_tag);
}


void create_guild(int actor_node, char *guild_name, char *guild_tag, int permission_level){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    //loop from 1 as guild 0 is reserved for unguilded players
    for(int i=1; i<MAX_GUILDS; i++){

        //check for duplicate guild tag/name
        if(strcmp_upper(guild_tag, guilds.guild[i].guild_tag)==0){

            send_text(socket, CHAT_SERVER, "%c a guild with that tag already exists", c_red3+127);
            return;
        }

        //check for duplicate guild name
        if(strcmp_upper(guild_name, guilds.guild[i].guild_tag)==0){

            send_text(socket, CHAT_SERVER, "%ca guild with that name already exists", c_red3+127);
            return;
        }
    }

    //loop from 1 as guild 0 is reserved for unguilded players
    for(int i=1; i<MAX_GUILDS; i++){

        //find spare id to insert the new guild
        if(strcmp(guilds.guild[i].guild_tag, "")==0){

            strcpy(guilds.guild[i].guild_name, guild_name);
            strcpy(guilds.guild[i].guild_tag, guild_tag);
            guilds.guild[i].guild_tag_colour=c_grey1;
            guilds.guild[i].date_guild_created=time(NULL);
            guilds.guild[i].permission_level=permission_level;
            guilds.guild[i].status=GUILD_ACTIVE;

            log_event(EVENT_SESSION, "create guild [%s]", guild_tag);

            send_text(socket, CHAT_SERVER, "%cyou have created guild %s", c_green3+127, guild_tag);

            push_sql_command("INSERT INTO GUILD_TABLE (GUILD_ID, GUILD_NAME, GUILD_TAG, GUILD_TAG_COLOUR, DATE_GUILD_CREATED, PERMISSION_LEVEL, STATUS) VALUES(%i, '%s', '%s', %i, '%s', %i, %i, %i)", \
                i, \
                guilds.guild[i].guild_name, \
                guilds.guild[i].guild_tag, \
                guilds.guild[i].guild_tag_colour, \
                (int)guilds.guild[i].date_guild_created, \
                guilds.guild[i].permission_level, \
                guilds.guild[i].status);

            return;
        }
    }

    send_text(socket, CHAT_SERVER, "%cmaximum number of guilds supported by the server has been exceeded. Inform server operator", c_red3+127);
    log_event(EVENT_ERROR, "maximum number of guilds has been exceeded");
}


void kick_guild_member(int actor_node, char *guild_tag, char *char_name){

    int socket=clients.client[actor_node].socket;

    //check guild tag exists
    int guild_id=get_guild_id(guild_tag);

    if(guild_id==-1){

        send_text(socket, CHAT_SERVER, "%cguild tag does not exist", c_red3+127);
        return;
    }

    //check char name exists
    if(get_db_char_data(char_name, 0)==false){

        log_event(EVENT_ERROR, "char[%s] does not exist", char_name);
        stop_server();
    }

    // check if char is already in a guild
    if(character.guild_id!=get_guild_id(guild_tag)){

        send_text(socket, CHAT_SERVER, "%c%s is not a member of guild %s", c_red3+127, char_name, guild_tag);
        return;
    }

    update_guild_details(character.character_id, 0, 0, 0);
    log_event(EVENT_SESSION, "char [%s] kicked from guild [%s]", char_name, guild_tag);
    send_text(socket, CHAT_SERVER, "%cyou kicked %s from your guild", c_green3+127, char_name);
}


void join_guild(int actor_node, char *char_name, char *guild_tag){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    // check guild tag exists
    int guild_id=get_guild_id(guild_tag);

    if(guild_id==-1){

        send_text(socket, CHAT_SERVER, "%cguild tag does not exist", c_red3+127);
        return;
    }

    //check char name exists
    if(get_db_char_data(char_name, 0)==false){

        send_text(socket, CHAT_SERVER, "%c%s does not exist", c_red3+127, char_name);
        return;
    }

    // check if char is already in a guild
    if(character.guild_id>0){

        send_text(socket, CHAT_SERVER, "%c%s is already a member of guild %s", c_red3+127, char_name, guilds.guild[character.guild_id].guild_tag);
        return;
    }

    //remove pending applications from this char to other guilds
    for(int i=0; i<MAX_GUILDS; i++){

        for(int j=0; j<MAX_GUILD_APPLICANTS; j++){

            if(strcmp(guilds.guild[i].applicant[j].char_name, char_name)==0){

                strcpy(guilds.guild[i].applicant[j].char_name, "");
                guilds.guild[i].applicant[j].application_date=0;
            }
        }
    }

    update_guild_details(character.character_id, guild_id, time(NULL), 0);

    log_event(EVENT_SESSION, "char [%s] accepted as member of guild [%s]", char_name, guild_tag);

    send_text(socket, CHAT_SERVER, "%cyou accepted %s as a member of your guild", c_green3+127, char_name);
}


void change_guild_rank(int actor_node, char *char_name, char *guild_tag, int new_rank){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    // check guild tag exists
    int guild_id=get_guild_id(guild_tag);

    if(guild_id==-1){

        send_text(socket, CHAT_SERVER, "%cguild tag does not exist", c_red3+127);
        return;
    }

    //check char name exists
    if(get_db_char_data(char_name, 0)==false){

        log_event(EVENT_ERROR, "char[%s] does not exist", char_name);
        stop_server();
    }

    // check if char is in guild
    if(character.guild_id!=guild_id){

        send_text(socket, CHAT_SERVER, "%c%s is not a member of guild %s", c_red3+127, char_name, guild_tag);
        return;
    }

    //check rank bounds
    if(new_rank<0 || new_rank>20){

        send_text(socket, CHAT_SERVER, "%cinvalid rank", c_red3+127);
        return;
    }

    //check rank permission
    if(new_rank<20 && new_rank>clients.client[actor_node].guild_rank-1){

        send_text(socket, CHAT_SERVER, "%cthat rank increase can only be granted by someone with guild rank %i or above", c_red3+127, new_rank+1);
        return;
    }

    //check guild master promotion permission
    if(new_rank==20 && clients.client[actor_node].guild_rank<20){

        send_text(socket, CHAT_SERVER, "%cthat rank increase can only be granted by someone with guild rank '20'", c_red3+127);
        return;
    }

    log_event(EVENT_SESSION, "char [%s] rank changed from [%i] to [%i] in guild [%s]", char_name, character.guild_rank, new_rank, guild_tag);
    send_text(socket, CHAT_SERVER, "%cyou have changed the rank of %s from %i to %i", c_green3+127, char_name, character.guild_rank, new_rank);

    //update guild details last of all otherwise details of previous rank will be lost
    update_guild_details(character.character_id, character.guild_id, character.joined_guild, new_rank);
}


void change_guild_permission(int actor_node, char *guild_tag, int permission_level){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    //does guild tag exist
    int guild_id=get_guild_id(guild_tag);

    if(guild_id==-1){

        send_text(socket, CHAT_SERVER, "%cguild %s does not exist", c_red3+127, guild_tag);
        return;
    }

    //bounds check permission level
    if(permission_level<PERMISSION_1 || permission_level>PERMISSION_3){

        send_text(socket, CHAT_SERVER, "%cinvalid guild permission level", c_red3+127);
        return;
    }

    //update database
    push_sql_command("UPDATE GUILD_TABLE SET PERMISSION_LEVEL=%i WHERE GUILD_ID=%i", permission_level, guild_id);

    send_text(socket, CHAT_SERVER, "%cyou have changed the permission level for guild %s to %i", c_green3+127, guild_tag, permission_level);
}


void list_guild_members(int actor_node, int order){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;
    int guild_id=clients.client[actor_node].guild_id;

    get_db_guild_member_list(guild_id, order);

    for(int i=0; i<guild_member_list.guild_member_count; i++){

        char date_stamp_str[50]="";
        get_date_stamp_str(guild_member_list.guild_member[i].date_joined_guild, date_stamp_str);

        char time_stamp_str[50]="";
        get_time_stamp_str(guild_member_list.guild_member[i].date_joined_guild, time_stamp_str);

        send_text(socket, CHAT_SERVER, "%c%s %s %s %i", c_green3+127,
            guild_member_list.guild_member[i].character_name,
            date_stamp_str,
            time_stamp_str,
            guild_member_list.guild_member[i].guild_rank);
    }
}


void send_guild_details(int actor_node, int guild_id){

    /** public function - see header */
    int socket=clients.client[actor_node].socket;

    if(guild_id==0){

        send_text(socket, CHAT_SERVER, "%c%s", c_red3+127, "you are a guildless player");
        return;
    }

    send_text(socket, CHAT_SERVER, "%cGuild       :%s", c_green3+127, guilds.guild[guild_id].guild_name);
    send_text(socket, CHAT_SERVER, "%cTag         :%c%s", c_green3+127, guilds.guild[guild_id].guild_tag_colour+127, guilds.guild[guild_id].guild_tag);
    send_text(socket, CHAT_SERVER, "%cDescription :%s", c_green3+127, guilds.guild[guild_id].guild_description);

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";
    get_time_stamp_str(guilds.guild[guild_id].date_guild_created, time_stamp_str);
    get_date_stamp_str(guilds.guild[guild_id].date_guild_created, date_stamp_str);
    send_text(socket, CHAT_SERVER, "%cDate Created :%s %s", c_green3+127, date_stamp_str, time_stamp_str);

    //list count members and list guild masters
    sqlite3_stmt *stmt;
    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE GUILD_ID=%i", guild_id);

// TODO (themuntdregger#1#): transfer to idle buffer
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    int member_count=0;
    char guild_masters[1024]="";

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //list the guild masters (chars with rank 20)
        if(sqlite3_column_int(stmt, 13)==20){

            sprintf(guild_masters, "%s %s", guild_masters, sqlite3_column_text(stmt, 1));
        }

        member_count++;
    }

    send_text(socket, CHAT_SERVER, "%cGuild Masters:%s", c_green3+127, guild_masters);
    send_text(socket, CHAT_SERVER, "%cMember Count :%i", c_green3+127, member_count);

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, sql);
    }
}
