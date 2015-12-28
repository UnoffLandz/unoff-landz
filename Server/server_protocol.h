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

#ifndef SERVER_PROTOCOL_H_INCLUDED
#define SERVER_PROTOCOL_H_INCLUDED

enum { // server to client protocol

    RAW_TEXT=0,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4    Uint8               channel
        5-   (null terminated    text
             char array)
*/

    ADD_ACTOR=2,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-4  uint16_t              character-id (=same as socket connection number)
        5    Uint8               move command
*/
    YOU_ARE=3,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-4  uint16_t              character id (same as client sock number)
*/

    SYNC_CLOCK=4,
    NEW_MINUTE=5,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-4  uint16_t              character-id (=same as socket connection number)
        4-5  uint16_t              minutes since midnight
*/

    REMOVE_ACTOR=6,
 /*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-4  uint16_t              character-id (=same as socket connection number)
*/

    CHANGE_MAP=7,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-   (null terminated    map elm file name
             char array)
*/

    HERE_YOUR_STATS=18,

    HERE_YOUR_INVENTORY=19,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        4    Uint8               slot count

        REPEAT FOR EACH SLOT...
        5-6 { uint16_t              object id
        7-11{ Uint32              amount
        12  { Uint8               slot
        13  { Uint8               flags
*/

    INVENTORY_ITEM_TEXT=20,
/*
         byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16_t            data-length (=packet length-2)
        4-   char array          text
*/


   GET_NEW_INVENTORY_ITEM=21, /* NOT IMPLEMENTED AS COVERED BY HERE_YOUR_INVENTORY.....

         byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16_t            data-length (=packet length-2)
        4-5  Uint16_t            image_id
        6-10 Uint32_t            quantity

        [on certain items]...
        11-12 Uint16_t           item uid
*/

    HERE_YOUR_GROUND_ITEMS=23, //opens bag
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4    Uint8               slot count

        REPEAT FOR EACH SLOT...
        5-6 { uint16_t            object id
        7-11{ Uint32              amount
        12  { Uint8               slot
*/

    GET_NEW_GROUND_ITEM=24,

    REMOVE_ITEM_FROM_GROUND=25,

    CLOSE_BAG=26, //closes bag inventory grid
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
*/

    GET_NEW_BAG=27, //creates empty bag
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4-5  uint16_t            x-pos on map
        6-7  uint16_t            y-pos on map
        8    Uint8               bag list number
*/
    GET_BAGS_LIST=28, //sends list of bags to client
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16_t            data-length (=packet length-2)
        4    Uint8_t             bag count

        for each bag in list:
            Uint16_t            x-pos on map
            Uint16_t            y-pos on map
            Uint8               bag list number
*/

    DESTROY_BAG=29,

    NPC_TEXT=30, //sends text to an npc dialogue box
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4-   (null terminated    text
             char array)
*/

    NPC_OPTIONS_LIST=31, //sends options to an npc dialogue box
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)

        array consisting of:
        Uint16                   length of option text
        (null terminated         option text
        char array)
        Uint16                   response code (0-99)
        Uint16                   actor id (actor=npc)
*/

    SEND_NPC_INFO=33, //sends npc name and portrait to an npc dialogue box
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16_t            data-length (=packet length-2)

             (null terminated    npc name
             char array)
        +1   Uint8               npc portrait id
*/

    ADD_NEW_ENHANCED_ACTOR=51,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-4  uint16_t              character-id (=same as socket connection number)
        4-5  uint16_t              x-position on map
        5-6  uint16_t              y-position on map
        6-7  uint16_t              z-position on map
        7-8  uint16_t              rotation angle
        9    Uint8               character type (ie male human)
        10   Uint8               unused
        11   Uint8               skin type
        12   Uint8               hair type
        13   Uint8               shirt type
        14   Uint8               pants type
        15   Uint8               boots type
        16   Uint8               head type
        17   Uint8               shield type
        18   Uint8               weapon type
        19   Uint8               cape type
        20   Uint8               helmet type
        21   Uint8               frame type
        22   uint16_t              max health
        23   uint16_t              current health
        24   Uint8               player type (human player=1, NPC=??)
        25-  (null terminated    character and guild name separated by a space and 127+colour codename
             char array)
        26   Unknown
        27   Uint8               character size
        28   Uint8               mount (nothing=255  brown horse=200)
        29   Uint8               neck attachment
*/

    DISPLAY_CLIENT_WINDOW=63,
    GET_ACTIVE_CHANNELS=71,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4    Unit8               active channel slot

        REPEAT FOR EACH SLOT...
        5-8  Uint32              channel slot
*/

    YOU_DONT_EXIST=249,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
*/

    LOG_IN_OK=250,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
*/

    LOG_IN_NOT_OK=251,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
*/

    CREATE_CHAR_OK=252,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
*/

    CREATE_CHAR_NOT_OK=253
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
*/

};

#endif // SERVER_PROTOCOL_H_INCLUDED
