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

#ifndef CHARACTERS_H_INCLUDED
#define CHARACTERS_H_INCLUDED

#include "clients.h"

#define CHARACTER_FILE "character.lst"

enum {// actor frame (needed for stand up/sit down

/*
    #define frame_walk 0
    #define frame_run 1
    #define frame_die1 2
    #define frame_die2 3
    #define frame_pain1 4
    #define frame_pain2 11
    #define frame_pick 5
    #define frame_drop 6
    #define frame_idle 7
    #define frame_harvest 8
    #define frame_cast 9
    #define frame_ranged 10
*/
    frame_sit=12,
    frame_stand=13,
    frame_sit_idle=14, //use this for sitting
/*
    #define frame_combat_idle 15
    #define frame_in_combat 16
    #define frame_out_combat 17
    #define frame_attack_up_1 18
    #define frame_attack_up_2 19
    #define frame_attack_up_3 20
    #define frame_attack_up_4 21
    #define frame_attack_down_1 22
    #define frame_attack_down_2 23
    #define frame_attack_down_3 24
    #define frame_attack_down_4 25
    #define frame_attack_down_5 26
    #define frame_attack_down_6 27
    #define frame_attack_down_7 28
    #define frame_attack_down_8 29
    #define frame_attack_down_9 30
    #define frame_attack_down_10 31
    #define frame_attack_up_5 32
    #define frame_attack_up_6 33
    #define frame_attack_up_7 34
    #define frame_attack_up_8 35
    #define frame_attack_up_9 36
    #define frame_attack_up_10 37
    //frame values for poses (40 different poses)
    #define frame_poses_start 100
    #define frame_poses_end 140
*/
};


enum { // actor command
    actor_cmd_nothing=0,
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
    QUIVER_BOLTS=13
};


enum {//types of equipable items

    KIND_OF_WEAPON      =0,
    KIND_OF_SHIELD      =1,
    KIND_OF_CAPE        =2,
    KIND_OF_HELMET      =3,
    KIND_OF_LEG_ARMOR   =4,
    KIND_OF_BODY_ARMOR  =5,
    KIND_OF_BOOT_ARMOR  =6,
    KIND_OF_NECK        =7,
};

enum {//equipable weapons

    WEAPON_NONE                 =0,
    SWORD_1                     =1,
    SWORD_2                     =2,
    SWORD_3                     =3,
    SWORD_4                     =4,
    SWORD_5                     =5,
    SWORD_6                     =6,
    SWORD_7                     =7,
    STAFF_1                     =8,
    STAFF_2                     =9,
    STAFF_3                     =10,
    STAFF_4                     =11,
    HAMMER_1                    =12,
    HAMMER_2                    =13,
    PICKAXE                     =14,
    SWORD_1_FIRE                =15,
    SWORD_2_FIRE                =16,
    SWORD_2_COLD                =17,
    SWORD_3_FIRE                =18,
    SWORD_3_COLD                =19,
    SWORD_3_MAGIC               =20,
    SWORD_4_FIRE                =21,
    SWORD_4_COLD                =22,
    SWORD_4_MAGIC               =23,
    SWORD_4_THERMAL             =24,
    SWORD_5_FIRE                =25,
    SWORD_5_COLD                =26,
    SWORD_5_MAGIC               =27,
    SWORD_5_THERMAL             =28,
    SWORD_6_FIRE                =29,
    SWORD_6_COLD                =30,
    SWORD_6_MAGIC               =31,
    SWORD_6_THERMAL             =32,
    SWORD_7_FIRE                =33,
    SWORD_7_COLD                =34,
    SWORD_7_MAGIC               =35,
    SWORD_7_THERMAL             =36,
    PICKAXE_MAGIC               =37,
    BATTLEAXE_IRON              =38,
    BATTLEAXE_STEEL             =39,
    BATTLEAXE_TITANIUM          =40,
    BATTLEAXE_IRON_FIRE         =41,
    BATTLEAXE_STEEL_COLD        =42,
    BATTLEAXE_STEEL_FIRE        =43,
    BATTLEAXE_TITANIUM_COLD     =44,
    BATTLEAXE_TITANIUM_FIRE     =45,
    BATTLEAXE_TITANIUM_MAGIC    =46,
    GLOVE_FUR                   =47,
    GLOVE_LEATHER               =48,
    BONE_1                      =49,
    STICK_1                     =50,
    SWORD_EMERALD_CLAYMORE      =51,
    SWORD_CUTLASS               =52,
    SWORD_SUNBREAKER            =53,
    SWORD_ORC_SLAYER            =54,
    SWORD_EAGLE_WING            =55,
    SWORD_RAPIER                =56,
    SWORD_JAGGED_SABRE          =57,
    SWORD_BRONZE                =58,
    BOW_LONG                    =64,
    BOW_SHORT                   =65,
    BOW_RECURVE                 =66,
    BOW_ELVEN                   =67,
    BOW_CROSS                   =68,
};

enum { //neck items

    NECK_NO_ITEM                =0,
    NECK_UNICORN_MEDALION       =1,
    NECK_SUN_MEDALLION          =2,
    NECK_BLUESTONE              =3,
    NECK_REDSTONE               =4,
    NECK_BRONZE_ARMOUR          =5,
    NECK_BLUE_ARMOUR            =6,
    NECK_RED_ARMOUR             =7,
    NECK_SILVER_TURQUIOSE       =8,
    NECK_TURQUIOSE_AMBER        =9,
    NECK_STAR_MEDALLION         =10,
    NECK_MOON_MEDALLION         =11,
    NECK_GOLD_MEDALLION         =12,
    NECK_FOX_SCARF              =13
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
    CAPE_NONE
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
    HELMET_NONE
};

extern struct client_node_type character;

/** RESULT  : calculates the visual range of a char

    RETURNS : the visual range

    PURPOSE : required by broadcast_add_new_enhanced_actor_packet

    NOTES   :
*/
int get_char_visual_range(int socket);


/** RESULT  : determines if a char is in-game

    RETURNS : NOT_FOUND(-1) if not on game or the connection number if the char is in game

    PURPOSE : required by send_pm

    NOTES   :
*/
int char_in_game(char *char_name);


/*****************************************************************************************************
***                                   C FUNCTIONS CALLED FROM C++ MODULES                          ***
******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT  : determines how long a char has been in game since creation

    RETURNS : time in seconds

    PURPOSE :

    NOTES   :
*/
int char_age(int socket);

/** RESULT  : sends char details to client in response to #details command

    RETURNS : void

    PURPOSE : used in process_idle_buffer2

    NOTES   :
*/
void send_char_details(int socket, const char *char_name);


#ifdef __cplusplus
}
#endif

#endif // CHARACTERS_H_INCLUDED
