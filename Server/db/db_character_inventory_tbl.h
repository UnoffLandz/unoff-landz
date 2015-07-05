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

#ifndef DB_CHARACTER_INVENTORY_TBL_H_INCLUDED
#define DB_CHARACTER_INVENTORY_TBL_H_INCLUDED

#define INVENTORY_TABLE_SQL "CREATE TABLE INVENTORY_TABLE( \
        ID                  INTEGER PRIMARY KEY    AUTOINCREMENT, \
        CHAR_ID             INT, \
        SLOT                INT, \
        IMAGE_ID            INT, \
        AMOUNT              INT)"

/** RESULT  :   creates a record of inventory contents for a new character in the inventory table

    RETURNS :   void

    PURPOSE :   creates a series of entries on the inventory_table corresponding to the value of
                each element in the client_inventory array contained within the character struct

    NOTES   :   1) Requires the character_id element of the character struct to be set in order for
                   the inventory slot entries to be linked to a character record. This is set by the
                   add_db_char_data function which, should be used before this function is called

                2) Inventory information is taken from the client_inventory element of the character
**/
void add_db_char_inventory(struct client_node_type character);


/** RESULT  : loads inventory data from the inventory table into the character struct

    RETURNS : void

    PURPOSE : Loads inventory data from the database to memory.

    NOTES   :
**/
void get_db_char_inventory(int character_id);

#endif // DB_CHARACTER_INVENTORY_TBL_H_INCLUDED
