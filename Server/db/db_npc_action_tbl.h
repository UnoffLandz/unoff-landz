#ifndef DB_NPC_ACTION_TBL_H_INCLUDED
#define DB_NPC_ACTION_TBL_H_INCLUDED

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

#define NPC_ACTION_TABLE_SQL "CREATE TABL E NPC_ACTION_TABLE( \
        NPC_ACTION_ID          INTEGER PRIMARY KEY     NOT NULL, \
        ACTION_TYPE            INT, \
        NPC_TEXT               TEXT, \
        OPTIONS_LIST           TEXT, \
        TEXT_SUCCESS           TEXT, \
        TEXT_FAIL              TEXT, \
        CHOICE                 INT
        OBJECT_ID_REQUIRED     INT, \
        OBJECT_AMOUNT_REQUIRED INT, \
        OBJECT_ID_GIVEN        INT, \
        OBJECT_AMOUNT_GIVEN    INT, \
        BOAT_NODE              INT, \
        DESTINATION            INT)"

/** RESULT  : loads data from the npc_action table into the npc action array

    RETURNS : void

    PURPOSE : retrieve npc action data from permanent storage

    NOTES   :
**/
void load_db_npc_action();


/** RESULT  : adds an npc trigger to the npc trigger table

    RETURNS : void

    PURPOSE :

    NOTES   :
**/
void add_db_npc_action(int npc_trigger_id, int trigger_type, int trigger_time,
int selection_option, int action_node);



#endif // DB_NPC_ACTION_TBL_H_INCLUDED
