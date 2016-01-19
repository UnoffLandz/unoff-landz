#ifndef DB_NPC_TRIGGER_TBL_H_INCLUDED
#define DB_NPC_TRIGGER_TBL_H_INCLUDED

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

#define NPC_TRIGGER_TABLE_SQL "CREATE TABLE NPC_TRIGGER_TABLE( \
        NPC_TRIGGER_ID    INTEGER PRIMARY KEY     NOT NULL, \
        TRIGGER_TYPE      INT, \
        TRIGGER_TIME      INT, \
        SELECT_OPTION     INT, \
        ACTION_NODE       INT)"

/** RESULT  : loads data from the npc_trigger table into the npc trigger array

    RETURNS : void

    PURPOSE : retrieve npc trigger data from permanent storage

    NOTES   :
**/
void load_db_npc_triggers();


/** RESULT  : adds an npc trigger to the npc trigger table

    RETURNS : void

    PURPOSE :

    NOTES   :
**/
void add_db_npc_trigger(int npc_trigger_id, int trigger_type, int trigger_time,
int selection_option, int action_node);

#endif // DB_NPC_TRIGGER_TBL_H_INCLUDED
