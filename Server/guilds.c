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

bool join_guild(int guild_id, int char_id){

    int date_joined_guild=time(NULL);

    //check char_id exists in database
    if(get_db_char_exists(char_id)==false){

        log_event(EVENT_ERROR, "char[%i] does not exist", char_id);
        stop_server();
    }

    //update database
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET GUILD_ID=%i, JOINED_GUILD=%i WHERE CHAR_ID=%i", guild_id, date_joined_guild, char_id);
    push_sql_command(sql);

    //if char in game then update client struct with guild data
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

            return true;
        }
    }

    return true;
}

void create_guild(int connection, char *guild_name, char *guild_tag, int permission_level){

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
            guilds.guild[i].guild_tag_colour=c_grey4;
            guilds.guild[i].date_guild_created=time(NULL);
            guilds.guild[i].permission_level=permission_level;
            guilds.guild[i].status=GUILD_ACTIVE;

            log_event(EVENT_SESSION, "create guild [%s]", guild_tag);

            sprintf(text_out, "%cyou have created the guild", c_green3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            char sql[MAX_SQL_LEN]="";

            snprintf(sql, MAX_SQL_LEN, "INSERT INTO GUILD_TABLE (GUILD_ID, GUILD_NAME, GUILD_TAG, GUILD_TAG_COLOUR, DATE_GUILD_CREATED, PERMISSION_LEVEL, STATUS) VALUES(%i, '%s', '%s', %i, %i, %i, %i)", \
                i, \
                guilds.guild[i].guild_name, \
                guilds.guild[i].guild_tag, \
                guilds.guild[i].guild_tag_colour, \
                (int)guilds.guild[i].date_guild_created, \
                guilds.guild[i].permission_level, \
                guilds.guild[i].status);

            push_sql_command(sql);

            return;
        }
    }

    sprintf(text_out, "%c maximum number of guilds supported by the server has been exceeded", c_red3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    return;
}
