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

#ifndef DB_CHARACTER_TBL_H_INCLUDED
#define DB_CHARACTER_TBL_H_INCLUDED

#define CHARACTER_TABLE_SQL "CREATE TABLE CHARACTER_TABLE( \
        CHAR_ID             INTEGER PRIMARY KEY AUTOINCREMENT, \
        CHAR_NAME           TEXT            NOT NULL, \
        PASSWORD            TEXT            NOT NULL, \
        CHAR_STATUS         INT, \
        ACTIVE_CHAN         INT, \
        CHAN_0              INT, \
        CHAN_1              INT, \
        CHAN_2              INT, \
        PLAYER_TYPE         INT, \
        UNUSED              INT, \
        MAP_ID              INT, \
        MAP_TILE            INT, \
        GUILD_ID            INT, \
        GUILD_RANK          INT, \
        CHAR_TYPE           INT, \
        SKIN_TYPE           INT, \
        HAIR_TYPE           INT, \
        SHIRT_TYPE          INT, \
        PANTS_TYPE          INT, \
        BOOTS_TYPE          INT, \
        HEAD_TYPE           INT, \
        SHIELD_TYPE         INT, \
        WEAPON_TYPE         INT, \
        CAPE_TYPE           INT, \
        HELMET_TYPE         INT, \
        FRAME               INT, \
        MAX_HEALTH          INT, \
        CURRENT_HEALTH      INT, \
        LAST_IN_GAME        INT, \
        CHAR_CREATED        INT, \
        JOINED_GUILD        INT, \
        PHYSIQUE_PP         INT, \
        VITALITY_PP         INT, \
        WILL_PP             INT, \
        COORDINATION_PP     INT, \
        OVERALL_EXP         INT, \
        HARVEST_EXP         INT, \
        HARVEST_LVL         INT, \
        INVENTORY           BLOB)"

#include "../clients.h"

/** RESULT  : determines if a char_id exists in the database CHARACTER_TABLE

    RETURNS : true/false

    PURPOSE : used in join_guild function

    NOTES   :
**/
bool get_db_char_exists(int char_id);


/** RESULT  : Retrieves char data from the Character Table and places it in the Character struct

    RETURNS : true/false

    PURPOSE : retrieves char data from the database based on char name or char id

    NOTES   : either char name or char_id must be specified.
**/
int add_db_char_data(struct client_node_type character);


/** RESULT  : Fetches the name and date of last character to be created from the Character Table and
              updates these to the name_last_char_created and date_last_char_created elements of the
              game_data struct

    RETURNS : void

    PURPOSE : Buffers database values in memory to enable faster access by function get_motd_header
              in module server_messaging.c

    NOTES   :
**/
void get_db_last_char_created();


/** RESULT  : Gets a count of entries in the Character Table

    RETURNS : The number of entries in the Character Table

    PURPOSE : Called at server boot to update values in game_data array. This is then used to provide
              MOTD info

    NOTES   :
**/
int get_db_char_count();


/** RESULT  : loads char data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of char data

    NOTES   :
*/
void batch_add_characters(char *file_name);


/*****************************************************************************************************
***                                   C FUNCTIONS CALLED FROM C++ MODULES                          ***
******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT  : loads data from the character table into the client array

    RETURNS : FOUND if char with the char_name is found in database, else NOT_FOUND

    PURPOSE : Loads character data from the database to memory.
              Also used to check if a new character name duplicates an existing character

    NOTES   :
**/
bool get_db_char_data(const char *char_name, int char_id);


/** RESULT  : loads npcs from the client table into the client array

    RETURNS : void

    PURPOSE : retrieve npc data from permanent storage

    NOTES   :
**/
void load_npc_characters();

/** RESULT  : updates the database with the actors inventory

    RETURNS : void

    PURPOSE : permanent storage of changes to that actors inventory

    NOTES   :
**/
void db_update_inventory(int actor_node);


#ifdef __cplusplus
}
#endif

#endif // DB_CHARACTER_TBL_H_INCLUDED
