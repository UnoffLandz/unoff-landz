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

#ifndef DB_CHAT_CHANNEL_TBL_H_INCLUDED
#define DB_CHAT_CHANNEL_TBL_H_INCLUDED

#define CHANNEL_TABLE_SQL "CREATE TABLE CHANNEL_TABLE( \
        CHANNEL_ID          INTEGER PRIMARY KEY     NOT NULL, \
        TYPE                INT,  \
        OWNER_ID            INT,  \
        PASSWORD            TEXT, \
        NAME                TEXT, \
        DESCRIPTION         TEXT, \
        NEw_CHARS           INT \
        )"

/** RESULT  : loads data from the channel table into the channel array

    RETURNS : number of rows read from the channel table

    PURPOSE : Loads channel data from the database to memory.

    NOTES   :
**/
int load_db_channels();


/** RESULT  : adds a channel to the channel table

    RETURNS : void

    PURPOSE : a test function to add channels to the channel table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_channel(int channel_id, int owner_id, int channel_type, char *password, char *channel_name, char*channel_description, int new_chars);

#endif // DB_CHAT_CHANNEL_TBL_H_INCLUDED
