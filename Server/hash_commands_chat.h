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

#ifndef HASH_COMMANDS_CHAT_H_INCLUDED
#define HASH_COMMANDS_CHAT_H_INCLUDED

/** RESULT  : handles join channel hash command

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
int hash_jc(int actor_node, char *text);


/** RESULT  : handles leave channel hash command

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
int hash_lc(int actor_node, char *text);


/** RESULT  : handles channel list hash command

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
int hash_cl(int actor_node, char *text);


/** RESULT  : handles channel participants hash command

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
int hash_cp(int actor_node, char *text);


/** RESULT  : handles private message hash command

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
int hash_pm(int actor_node, char *text);

#endif // HASH_COMMANDS_CHAT_H_INCLUDED
