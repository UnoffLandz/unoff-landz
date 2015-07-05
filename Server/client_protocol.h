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
    MOVE_INVENTORY_ITEM=20,
    HARVEST=21,
    DROP_ITEM=22,
    PICK_UP_ITEM=23,
    INSPECT_BAG=25,
    LOOK_AT_MAP_OBJECT=27,
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
