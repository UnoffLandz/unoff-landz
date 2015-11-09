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

#ifndef HASH_COMMANDS_GUILDS_H_INCLUDED
#define HASH_COMMANDS_GUILDS_H_INCLUDED


/** RESULT  : changes the description of a guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_guild_description(int connection, char *text);


/** RESULT  : handles guild message hash command

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
int hash_guild_message(int connection, char *text);


/** RESULT  : guild information

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_guild_details(int connection, char *text);


/** RESULT  : lists guild members

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_list_guild(int connection, char *text);


/** RESULT  : kicks a member from a a guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_kick_guild_member(int connection, char *text);


/** RESULT  : accepts a chars application to join a guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_reject_applicant(int connection, char *text);


/** RESULT  : accepts a chars application to join a guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_accept_applicant(int connection, char *text);


/** RESULT  : lists characters who have applied for guild membership

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_list_applicants(int connection, char *text);


/** RESULT  : change a chars guild rank

    RETURNS : void

    PURPOSE :

    NOTES   : rank 18+ guild members only
*/
int hash_change_rank(int connection, char *text);


/** RESULT  : application by char to join guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_apply_guild(int connection, char *text);


/** RESULT  : changes guild tag colour

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_guild_tag_colour(int connection, char *text);


/** RESULT  : char leaves guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_leave_guild(int connection, char *text);


#endif // HASH_COMMANDS_GUILDS_H_INCLUDED
