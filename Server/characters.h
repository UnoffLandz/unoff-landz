/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

#ifndef CHARACTERS_H_INCLUDED
#define CHARACTERS_H_INCLUDED

#include "clients.h"

enum { //char status
    CHAR_ALIVE,
    CHAR_DEAD,
    CHAR_BANNED
};

enum {// actor frame (needed for stand up/sit down
    frame_sit=12,
    frame_stand=13,
    frame_sit_idle=14,
};

enum { // actor command
    actor_cmd_sit_down=13,
    actor_cmd_stand_up=14,
    actor_cmd_move_n=20,
    actor_cmd_move_ne=21,
    actor_cmd_move_e=22,
    actor_cmd_move_se=23,
    actor_cmd_move_s=24,
    actor_cmd_move_sw=25,
    actor_cmd_move_w=26,
    actor_cmd_move_nw=27
};

enum { // shield type
    SHIELD_WOOD=0,
    SHIELD_WOOD_ENHANCED=1,
    SHIELD_IRON=2,
    SHIELD_STEEL=3,
    SHIELD_TITANIUM=4,
    SHIELD_BRONZE=5,
    QUIVER_ARROWS=7,
    SHIELD_NONE=11,
    QUIVER_BOLTS=13,
};

enum { // weapon type
    WEAPON_NONE=0,
    SWORD_1=1,
};

enum { // cape type
    CAPE_BLACK,
    CAPE_BLUE,
    CAPE_BLUEGRAY,
    CAPE_BROWN,
    CAPE_BROWNGRAY,
    CAPE_GRAY,
    CAPE_GREEN,
    CAPE_GREENGRAY,
    CAPE_PURPLE,
    CAPE_WHITE,
    CAPE_FUR,
    CAPE_GOLD,
    CAPE_RED,
    CAPE_ORANGE,
    CAPE_MOD,
    CAPE_DERIN,
    CAPE_RAVENOD,
    CAPE_PLACID,
    CAPE_LORD_VERMOR,
    CAPE_AISLINN,
    CAPE_SOLDUS,
    CAPE_LOTHARION,
    CAPE_LEARNER,
    CAPE_UNUSED_23,
    CAPE_UNUSED_24,
    CAPE_UNUSED_25,
    CAPE_UNUSED_26,
    CAPE_UNUSED_27,
    CAPE_UNUSED_28,
    CAPE_UNUSED_29,
    CAPE_NONE,
};

enum { // helmet type
    HELMET_IRON,
    HELMET_FUR,
    HELMET_LEATHER,
    HELMET_RACOON,
    HELMET_SKUNK,
    HELMET_CROWN_OF_MANA,
    HELMET_CROWN_OF_LIFE,
    HELMET_STEEL,
    HELMET_TITANIUM,
    HELMET_BRONZE,
    HELMET_UNUSED_10,
    HELMET_UNUSED_11,
    HELMET_UNUSED_12,
    HELMET_UNUSED_13,
    HELMET_UNUSED_14,
    HELMET_UNUSED_15,
    HELMET_UNUSED_16,
    HELMET_UNUSED_17,
    HELMET_UNUSED_18,
    HELMET_UNUSED_19,
    HELMET_NONE,
};

extern struct client_node_type character;

/** RESULT  : calculates the visual range of a char

    RETURNS : the visual range

    PURPOSE : required by broadcast_add_new_enhanced_actor_packet

    NOTES   :
*/
int get_char_visual_range(int connection);


/** RESULT  : determines if a char is in-game

    RETURNS : NOT_FOUND(-1) if not on game or the connection number if the char is in game

    PURPOSE : required by send_pm

    NOTES   :
*/
int char_in_game(char *char_name);


/** RESULT  : determines how long a char has been in game since creation

    RETURNS : time in seconds

    PURPOSE :

    NOTES   :
*/
int char_age(int connection);

#endif // CHARACTERS_H_INCLUDED
