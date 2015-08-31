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

#ifndef GUILD_H_INCLUDED
#define GUILD_H_INCLUDED

#include <time.h>
#include <stdbool.h>

#define MAX_GUILDS 10
#define MAX_GUILD_APPLICANTS 10
#define GUILD_OPERATORS 0 //defines the guild id for the Operators guild

enum{//guild status

    GUILD_ACTIVE,
    GUILD_CLOSED,
};

struct guild_node_type {

    char guild_name[80];
    char guild_tag[5];
    char guild_description[1024];
    int guild_tag_colour;
    time_t date_guild_created;
    int permission_level;
    int status;

    struct {

        char char_name[80];
        time_t application_date;
    }applicant[MAX_GUILD_APPLICANTS];
};

struct guild_list_type {

    struct guild_node_type guild[MAX_GUILDS];
};
extern struct guild_list_type guilds;


/** RESULT   : creates a guild

    RETURNS  : void

    PURPOSE  : used by function hash_create_guild

    NOTES    :
**/
void create_guild(int connection, char *guild_name, char *guild_tag, int permission_level);


/** RESULT   : application from a char for membership of a guild

    RETURNS  : void

    PURPOSE  : used by function hash_apply_guild

    NOTES    :
**/
void apply_guild(int connection, char *guild_tag);


/** RESULT   : joins a char to a guild

    RETURNS  : void

    PURPOSE  : used by function create database

    NOTES    :
**/
bool join_guild(int guild_id, int char_id);

#endif // GUILD_H_INCLUDED
