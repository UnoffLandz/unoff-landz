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

#ifndef SERVER_PROTOCOL_H_INCLUDED
#define SERVER_PROTOCOL_H_INCLUDED

enum { // server to client protocol
    RAW_TEXT=0,
    ADD_ACTOR=2,
    YOU_ARE=3,
    SYNC_CLOCK=4,
    NEW_MINUTE=5,
    REMOVE_ACTOR=6,
    CHANGE_MAP=7,
    HERE_YOUR_STATS=18,
    HERE_YOUR_INVENTORY=19,
    GET_NEW_INVENTORY_ITEM=21,
    HERE_YOUR_GROUND_ITEMS=23,
    GET_NEW_GROUND_ITEM=24,
    S_CLOSE_BAG=26,
    GET_NEW_BAG=27,
    DESTROY_BAG=29,
    ADD_NEW_ENHANCED_ACTOR=51,
    GET_ACTIVE_CHANNELS=71,
    YOU_DONT_EXIST=249,
    LOG_IN_OK=250,
    LOG_IN_NOT_OK=251,
    CREATE_CHAR_OK=252,
    CREATE_CHAR_NOT_OK=253
};

#endif // SERVER_PROTOCOL_H_INCLUDED
