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

#ifndef DB_GUILD_TBL_H_INCLUDED
#define DB_GUILD_TBL_H_INCLUDED

#define GUILD_TABLE_SQL "CREATE TABLE GUILD_TABLE( \
        GUILD_ID            INTEGER PRIMARY KEY     NOT NULL, \
        GUILD_NAME          TEXT, \
        GUILD_TAG           TEXT, \
        GUILD_TAG_COLOUR    INT, \
        GUILD_DESCRIPTION   TEXT, \
        DATE_GUILD_CREATED  INT, \
        PERMISSION_LEVEL    INT, \
        STATUS              INT  \
        )"

/** RESULT  : loads data from the guild table into the guild array

    RETURNS : void

    PURPOSE : retrieve guild data from permanent storage

    NOTES   :
**/
void load_db_guilds();


/** RESULT  : Adds a guild to the Guild Table of the database

    RETURNS : void

    PURPOSE : permanently store guild data

    NOTES   :
**/
int add_db_guild(char *guild_name, char *guild_tag, int guild_tag_colour, char *guild_description,
    int guild_permission_level,
    int guild_status);

#endif // DB_GUILD_TBL_H_INCLUDED
