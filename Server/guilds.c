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
#include "server_start_stop.h"
#include "idle_buffer2.h"
#include "characters.h"

struct guild_list_type guilds;

int get_guild_id(char *guild_tag){

    for(int i=0; i<MAX_GUILDS; i++){

        if(strcmp(guild_tag, guilds.guild[i].guild_tag)==0){

            return i;
        }
    }

    return -1;
}

void apply_guild(int connection, char *guild_tag){

    /** public function - see header **/

    int guild_id=get_guild_id(guild_tag);
    char text_out[80]="";

    if(guild_id==-1){

        sprintf(text_out, "%cguild %s does not exist", c_red3+127, guild_tag);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    for(int i=0; i<MAX_GUILD_APPLICANTS; i++){

        if(strcmp(guilds.guild[guild_id].applicant[i].char_name, "")==0){

            strcpy(guilds.guild[guild_id].applicant[i].char_name, clients.client[connection].char_name);
            guilds.guild[guild_id].applicant[i].application_date=time(NULL);

            sprintf(text_out, "%cyour application to join guild %s has been made. Now wait for the guild master to approve your application", c_green3+127, guild_tag);
            send_raw_text(connection, CHAT_SERVER, text_out);

            return;
        }
    }

    sprintf(text_out, "%csorry. Guild %s has too many pending applications. Please contact the guild master", c_red3+127, guild_tag);
    send_raw_text(connection, CHAT_SERVER, text_out);
}


void create_guild(int connection, char *guild_name, char *guild_description, char *guild_tag, int permission_level){

    /** public function - see header **/

    char text_out[80]="";

    for(int i=0; i<MAX_GUILDS; i++){

        //check for duplicate guild tag
        if(strcmp(guilds.guild[i].guild_tag, guild_tag)==0){

            sprintf(text_out, "%c a guild with that tag already exists", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            return;
        }

        //check for duplicate guild tag
        if(strcmp(guilds.guild[i].guild_name, guild_name)==0){

            sprintf(text_out, "%c a guild with that name already exists", c_red3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            return;
        }

        //find spare id to insert the new guild
        if(strcmp(guilds.guild[i].guild_tag, "")==0){

            strcpy(guilds.guild[i].guild_name, guild_name);
            strcpy(guilds.guild[i].guild_tag, guild_tag);
            strcpy(guilds.guild[i].guild_description, guild_description);
            guilds.guild[i].guild_tag_colour=c_grey4;
            guilds.guild[i].date_guild_created=time(NULL);
            guilds.guild[i].permission_level=permission_level;
            guilds.guild[i].status=GUILD_ACTIVE;

            log_event(EVENT_SESSION, "create guild [%s]", guild_tag);

            sprintf(text_out, "%cyou have created the guild", c_green3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            char sql[MAX_SQL_LEN]="";

            snprintf(sql, MAX_SQL_LEN, "INSERT INTO GUILD_TABLE (GUILD_ID, GUILD_NAME, GUILD_TAG, GUILD_TAG_COLOUR, GUILD_DESCRIPTION, DATE_GUILD_CREATED, PERMISSION_LEVEL, STATUS) VALUES(%i, '%s', '%s', %i, '%s', %i, %i, %i)", \
                i, \
                guilds.guild[i].guild_name, \
                guilds.guild[i].guild_tag, \
                guilds.guild[i].guild_tag_colour, \
                guilds.guild[i].guild_description, \
                (int)guilds.guild[i].date_guild_created, \
                guilds.guild[i].permission_level, \
                guilds.guild[i].status);

            push_sql_command(sql);

            return;
        }
    }

    sprintf(text_out, "%c maximum number of guilds supported by the server has been exceeded", c_red3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);
}


void kick_guild_member(int connection, char *guild_tag, char *char_name){

    char text_out[80]="";

    // check guild tag exists
    int guild_id=get_guild_id(guild_tag);
    if(guild_id==-1){

        sprintf(text_out, "%cguild tag does not exist", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check char name exists
    if(get_db_char_data(char_name, 0)==false){

        log_event(EVENT_ERROR, "char[%s] does not exist", char_name);
        stop_server();
    }

    // check if char is already in a guild
    if(character.guild_id!=get_guild_id(guild_tag)){

        sprintf(text_out, "%ccharacter is not a member of guild %s", c_red3+127, guilds.guild[character.guild_id].guild_tag);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    guild_id=0;
    int date_joined_guild=NULL;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET GUILD_ID=%i, JOINED_GUILD=%i WHERE CHAR_ID=%i", guild_id, date_joined_guild, character.character_id);
    push_sql_command(sql);

    //if char in game then update client struct
    for(int i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].character_id==character.character_id){

            clients.client[i].guild_id=guild_id;
            clients.client[i].joined_guild=date_joined_guild;

            log_event(EVENT_SESSION, "char [%s] kicked from guild [%s]", clients.client[i].char_name, guilds.guild[guild_id].guild_tag);

            //re-add char to map so that removed guild tag is immediately visible
            if(add_char_to_map(i, clients.client[i].map_id, clients.client[i].map_tile)==false){

               log_event(EVENT_ERROR, "unable to add char[%s] to map in function %s: module %s: line %i", clients.client[i].char_name, __func__, __FILE__, __LINE__);
               stop_server();
            }

            return;
        }
    }
}


void _join_guild(int guild_id, int char_id){

    /** public function - see header **/

    //update database
    int date_joined_guild=time(NULL);

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET GUILD_ID=%i, JOINED_GUILD=%i WHERE CHAR_ID=%i", guild_id, date_joined_guild, char_id);
    push_sql_command(sql);

    //if char in game then update client struct
    for(int i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].character_id==char_id){

            clients.client[i].guild_id=guild_id;
            clients.client[i].joined_guild=date_joined_guild;

            log_event(EVENT_SESSION, "char [%s] joined guild [%s]", clients.client[i].char_name, guilds.guild[guild_id].guild_tag);

            //re-add char to map so that guild tag is immediately visible
            if(add_char_to_map(i, clients.client[i].map_id, clients.client[i].map_tile)==false){

               log_event(EVENT_ERROR, "unable to add char[%s] to map in function %s: module %s: line %i", clients.client[i].char_name, __func__, __FILE__, __LINE__);
               stop_server();
            }

            return;
        }
    }
}

void join_guild(int connection, char *char_name, char *guild_tag){

    /** public function - see header **/

    char text_out[80]="";

    // check guild tag exists
    int guild_id=get_guild_id(guild_tag);
    if(guild_id==-1){

        sprintf(text_out, "%cguild tag does not exist", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check char name exists
    if(get_db_char_data(char_name, 0)==false){

        log_event(EVENT_ERROR, "char[%s] does not exist", char_name);
        stop_server();
    }

    // check if char is already in a guild
    if(character.guild_id>0){

        sprintf(text_out, "%ccharacter is already a member of guild %s", c_red3+127, guilds.guild[character.guild_id].guild_tag);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    _join_guild(character.character_id, guild_id);
}

void change_guild_rank(int connection, char *char_name, char *guild_tag, int guild_rank){

    /** public function - see header **/

    char text_out[80]="";

    // check guild tag exists
    int guild_id=get_guild_id(guild_tag);
    if(guild_id==-1){

        sprintf(text_out, "%cguild tag does not exist", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check char name exists
    if(get_db_char_data(char_name, 0)==false){

        log_event(EVENT_ERROR, "char[%s] does not exist", char_name);
        stop_server();
    }

    // check if char is in guild
    if(character.guild_id!=guild_id){

        sprintf(text_out, "%ccharacter is not a member of guild %s", c_red3+127, guilds.guild[character.guild_id].guild_tag);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check rank bounds
    if(guild_rank<0 || guild_rank>20){

        sprintf(text_out, "%cinvalid rank", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check rank permission
    if(guild_rank<20 && guild_rank>clients.client[connection].guild_rank-1){

        sprintf(text_out, "%cthat rank increase can only be granted by someone with guild rank %i or above", c_red3+127, guild_rank+1);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check guild master promotion permission
    if(guild_rank==20 && clients.client[connection].guild_rank<20){

        sprintf(text_out, "%cthat rank increase can only be granted by someone with guild rank 20", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //update database
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET GUILD_RANK=%i WHERE CHAR_ID=%i", guild_rank, character.character_id);
    push_sql_command(sql);

    //if char in game then update client struct
    for(int i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].character_id==character.character_id){

            clients.client[i].guild_rank=guild_rank;

            log_event(EVENT_SESSION, "char [%s] promoted to rank [%i] in guild [%s]", clients.client[i].char_name, guild_rank, guilds.guild[guild_id].guild_tag);

            return;
        }
    }
}

void change_guild_permission(int connection, char *guild_tag, int permission_level){

    /** public function - see header **/

    char text_out[80]="";

    //does guild tag exist
    int guild_id=get_guild_id(guild_tag);
    if(guild_id==-1){

        sprintf(text_out, "%cguild %s does not exist", c_red3+127, guild_tag);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //bounds check permission level
    if(permission_level<PERMISSION_1 || permission_level>PERMISSION_3){

        sprintf(text_out, "%cinvalid guild permission level", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //update database
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE GUILD_TABLE SET PERMISSION_LEVEL=%i WHERE GUILD_ID=%i", permission_level, guild_id);
    push_sql_command(sql);

    sprintf(text_out, "%cyou have changed the permission level for guild %s to %i", c_red3+127, guild_tag, permission_level);
    send_raw_text(connection, CHAT_SERVER, text_out);
}

