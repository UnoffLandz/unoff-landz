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

#ifndef CHARACTER_CREATION_H_INCLUDED
#define CHARACTER_CREATION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT  :   Checks whether a character name exists in the character_table of the database. If the
                name exists, character creation is aborted and a message sent to the client. If the
                name does not exist, the character creation packet is placed in the idle buffer with
                an instruction to add new character at the next idle event

    RETURNS :   void

    PURPOSE :   Code modularisation and logical grouping of related functions

    NOTES   :   Enables priority handling of client communications which, might otherwise be
                delayed by processing of database operations
**/
void check_new_character(int actor_node, const unsigned char *packet);


/** RESULT  :   Adds details of a new character to the database during an idle event

    RETURNS :   void

    PURPOSE :   Code modularisation and logical grouping of related functions

    NOTES   :   Enables priority handling of client communications which, might otherwise be
                delayed by processing of database operations
**/
void add_new_character(int actor_node, const unsigned char *packet);

#ifdef __cplusplus
}
#endif


#endif // CHARACTER_CREATION_H_INCLUDED
