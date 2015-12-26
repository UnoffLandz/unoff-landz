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

#ifndef CLIENT_PROTOCOL_H_INCLUDED
#define CLIENT_PROTOCOL_H_INCLUDED

#define EL_SEND_VERSION_LEN 17
#define OL_SEND_VERSION_LEN 21


enum { // client to server protocol
    RAW_TEXT=0,
    MOVE_TO=1,
    SEND_PM=2,
    GET_PLAYER_INFO=5,
    SIT_DOWN=7,
    SEND_ME_MY_ACTORS=8,
    SEND_OPENING_SCREEN=9,
    SEND_VERSION=10,
    HEARTBEAT=14,
    USE_OBJECT=16,
    LOOK_AT_INVENTORY_ITEM=19,
 /*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        4    Uint8               slot number
*/

    MOVE_INVENTORY_ITEM=20,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        4    Uint8               from slot number
        5    Uint8               to slot number

        n.b - slots 0-35 are used to hold stored items, and slots 36 to 43 hold wearable items
*/

    HARVEST=21,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4-6  Uint16_t            image id
        6-10 Uint32              Quantity
        11   Uint8               slot
        12   Uint8               flags

*/

    DROP_ITEM=22,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t              data-length (=packet length-2)
        3-   Uint8               inventory slot
        4-8  Uint32              amount
*/

    PICK_UP_ITEM=23,

    INSPECT_BAG=25,

    LOOK_AT_MAP_OBJECT=27,
/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4-8  Uint32              position of object in the map 3d object list
*/

    TOUCH_PLAYER=28,

/*
        byte word                purpose
        ---- ------------------  --------------------------------------------------------------
        1    Uint8               protocol
        2-3  uint16_t            data-length (=packet length-2)
        4-8  Uint32              actor id
*/

    RESPOND_TO_NPC=29,

    SEND_PARTIAL_STATS=49,
    PING_RESPONSE=60,
    SET_ACTIVE_CHANNEL=61,
    LOG_IN=140,
    CREATE_CHAR=141,
    GET_DATE=230,
    GET_TIME=231,
    SERVER_STATS=232
};

#endif // CLIENT_PROTOCOL_H_INCLUDED
