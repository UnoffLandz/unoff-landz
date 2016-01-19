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
#ifndef SERVER_BUILD_DETAILS_H_INCLUDED
#define SERVER_BUILD_DETAILS_H_INCLUDED


#define SERVER_NAME "OL Map Walker"
#define VERSION "UnoffLandz 4"
#define BUILD "9"
#define BUILD_DATE __DATE__

/***************************************************************************************************

Done - Fixed char sit/stand
Done - drop bag and other commands stop harvesting
Done - harvesting and other commands stop movement
Done - Fixed #gm
Done - NPC move script implemented
Done - #boat (reminds when boat leaves)
Done - trigger table, action table and boat table stubs
Done - replaced hard coded values for max char name and max char password
Done - Fixed bug unknown protocol packets are not being logged in packet log
Done - separate module for ops #commands
Done - separate module for devs #commands
Done - #TRACE_PATH implemented
Done - changed add_map function to remove supplementary details
Done - added hash commands so as map supplementary details can be added in-game
Done - added separate function to extract 3d object list from elm file
Done - fixed bug in get_nearest_unoccupied_tile
Done - #jump now contains option to move to first walkable tile
Done - new harvestables (including pumpkin
Done - batch load maps on startup from map.lst file
Done - batch load objects on startup from object.lst file
Done - batch load e3ds on startup from e3d.lst
Done - command line option to load e3ds/objects
Done - command line option to reload map objects
Done - #gm only available for ranks of 1 and greater
Done - add THREED_OBJECT_COUNT to map table------------------------*
Done - changed GM_PERMISSION to PLAYER_TYPE and IG PERMISSION to UNUSED in character table

***************************************************************************************************

                                TO - DO

TEST multiple guild application handling

for NPC's introduce time trigger to sell/boat ticket purchase so that different responses
can be engineered for different times of day.

load guilds, chars, channels, seasons, race, gender, char_type from text file
add npc wear item action
add npc sit/stand action
change all sql to use parameters rather than inserts
replace printf with fprintf(stderr

bag_proximity (reveal and unreveal) use destroy and create in place of revised client code
need #command to withdraw application to join guild
need #letter system to inform ppl if guild application has been approved/rejected also if guild member leaves
transfer server welcome message to the database
#command to change guild chan join/leave notification colours
remove character_type_name field from CHARACTER_TYPE_TABLE
map object reserve respawn
#command to #letter all members of a guild
finish script loading
#IG guild channel functionality
OPS #command to #letter all chars
need #command to #letter all guild members (guild master only)
implement guild stats
Table to separately record all drops/pick ups in db
Table to separately record chars leaving and joining guilds
save guild applicant list to database
document idle_buffer2.h
convert attribute struct so as attribute type can be addressed programatically
identify cause of stall after login (likely to be loading of inventory from db)
identify cause of char bobbing
put inventory slots in a binary blob (may solve stall on log in)
improve error handling on upgrade_database function
create circular buffer for receiving packets
need #function to describe char and what it is wearing)
document new database/struct relationships
finish char_race_stats and char_gender_stats functions in db_char_tbl.c
banned chars go to ban map (jail). Dead chars got to dead map (ghost and graveyard)

***************************************************************************************************/

#endif // SERVER_BUILD_DETAILS_H_INCLUDED
