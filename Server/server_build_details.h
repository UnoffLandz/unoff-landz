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
#define BUILD "10"
#define BUILD_DATE __DATE__

/***************************************************************************************************
                                BUILD 10

Done -  Implemented sqlite_prepare and sqlite_finalize reporting wrappers

Done -  add emu equipable_item_type and equipable_item_id to object table

Done -  refactored functions in database_functions.c

Done -  database functions now check to make sure database is open/closed

Done -  corrected database functions which did not test or incorrectly handled the
        return code of sqlite3_finalize statement

Done -  replaced printf with fprintf for console messages

Done -  implemented batch loading of char races, genders, char types, seasons

Done -  refactored functions in db_upgrade.c

Done - implemented carry capacity values on hud and inventory grid

Done - corrected bug spotted by Life where equipping more than 1 item causes client to
crash.

Done - implemented char object equipping

Done - #object command

Done - implemented skills exp and levels

Done - implemented batch loading of skills data

***************************************************************************************************

                                TO - DO

Need command line option to load skill
Fix bug duplicate skills table error

Put broadcast_get_new_bag_packet etc in new module broadcast_bags

TEST whether db skills add from list to database

implement prepare_sql and destroy_sql functions in place of raw sqlite functions
implement GET_CALL_INFO macro in log functions

function to calculate level from exp
Load exp/skills from list
BUG send_here_your_stats not working

TEST multiple guild application handling
TEST database upgrade
TEST map object upgrade

for NPC's introduce time trigger to sell/boat ticket purchase so that different responses
can be engineered for different times of day.

load game data, guilds, chars, channels and attributes from text file
add npc wear item action
add npc sit/stand action

determine how to add water lilly to item dds (object 673  items27.dds in textures plant1.dds in 3d objects)
implement nexus, pick-points (apply to equipable items)

harvest red bush, blue bush
implement fish
change all sql to use parameters rather than inserts (can't do this process_sql relies on
a sql string being passed to the function)
replace snprintf with ssnprintf
bag_proximity (reveal and unreveal) use destroy and create in place of revised client code
need #command to withdraw application to join guild
need #letter system to inform ppl if guild application has been approved/rejected also if guild member leaves
transfer server welcome message to the database
#command to change guild chan join/leave notification colours
remove character_type_name field from CHARACTER_TYPE_TABLE
map object reserve respawn
#command to #letter all members of a guild
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
create circular buffer for receiving packets
need #function to describe char and what it is wearing)
document new database/struct relationships
finish char_race_stats and char_gender_stats functions in db_char_tbl.c
banned chars go to ban map (jail). Dead chars got to dead map (ghost and graveyard)

***************************************************************************************************/

#endif // SERVER_BUILD_DETAILS_H_INCLUDED
