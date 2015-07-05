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
        2-3  Uint16              data-length (=packet length-2)
        4    Uint8               channel
        5-   (null terminated    text
             char array)
*/

    ADD_ACTOR=2,
/*      byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        3-4  Uint16              character-id (=same as socket connection number)
        5    Uint8               move command
*/
    YOU_ARE=3,

/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        3-4  Uint16              character id (same as client sock number)
*/

    SYNC_CLOCK=4,
    NEW_MINUTE=5,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        3-4  Uint16              character-id (=same as socket connection number)
        4-5  Uint16              minutes since midnight
*/

    REMOVE_ACTOR=6,
 /*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        3-4  Uint16              character-id (=same as socket connection number)
*/

    CHANGE_MAP=7,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        3-   (null terminated    map elm file name
             char array)
*/

    HERE_YOUR_STATS=18,
    HERE_YOUR_INVENTORY=19,
    GET_NEW_INVENTORY_ITEM=21,
    HERE_YOUR_GROUND_ITEMS=23,
    GET_NEW_GROUND_ITEM=24,
    S_CLOSE_BAG=26,
    GET_NEW_BAG=27,
    DESTROY_BAG=29,
    ADD_NEW_ENHANCED_ACTOR=51,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        3-4  Uint16              character-id (=same as socket connection number)
        4-5  Uint16              x-position on map
        5-6  Uint16              y-position on map
        6-7  Uint16              z-position on map
        7-8  Uint16              rotation angle
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
        22   Uint16              max health
        23   Uint16              current health
        24   Uint8               player type (human player=1, NPC=??)
        25-  (null terminated    character and guild name separated by a space and 127+colour codename
             char array)
        26   Unknown
        27   Uint8               character size
        28   Uint8               mount (nothing=255  brown horse=200)
        29   Uint8               neck attachment
*/
    GET_ACTIVE_CHANNELS=71,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
        4    Unit8               active channel slot
        5-   Uint8 array         channel slot
*/



    YOU_DONT_EXIST=249,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
*/

    LOG_IN_OK=250,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
*/

    LOG_IN_NOT_OK=251,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
*/

    CREATE_CHAR_OK=252,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
*/

    CREATE_CHAR_NOT_OK=253
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  Uint16              data-length (=packet length-2)
*/

};

#endif // SERVER_PROTOCOL_H_INCLUDED
