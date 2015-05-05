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

#ifndef GUILD_H_INCLUDED
#define GUILD_H_INCLUDED

#include <time.h>

#define MAX_GUILDS 10

struct guild_node_type {

    char guild_name[80];
    char guild_tag[5];
    int guild_tag_colour;
    time_t date_created;
};

struct guild_list_type {

    struct guild_node_type guild[MAX_GUILDS];
};
extern struct guild_list_type guilds;

#endif // GUILD_H_INCLUDED
