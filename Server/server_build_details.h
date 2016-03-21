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

#define VERSION "UnoffLandz 4"
#define BUILD "12"
#define BUILD_DATE __DATE__

/***************************************************************************************************
                                    BUILD 12

Done (48) Add char neck attachment to client array and database table

Done (46) Add actor scale to client array and database table

Fixed (61) chan details not shown during database creation

Done Refactored add_db_guild

Done Refactored add_db_char_data

Done (60) replace SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE") with sqlite3_get_last_insert_rowid

Dropped (59) replace get_db_char_count with char count field in game_data table (bad idea)

Done (40) change sql statements to use bind parameters rather than inserts

Done (39) implement prepare and destroy wrapper for sqlite functions

Dropped (17) map object reserve respawn (duplicate of 43)

Done (63) replace log_sqlite_error functions

Done added sqlite return code explanation to logging

Fixed (6) char bobbing now stopped

Done char now automatically stands and remains standing if moved from sitting position

Dropped (67) insert callback function within sqlite prepare/step/finalise (won't work)

Done refactored protocol array to remove redundant placeholders

Done refactored get_db_char_data function

Partial (65) added sqlite_exec to skills, attributes, races, genders, char_types e3d, maps
when loading from file to database

Done (69) change char sql[MAX_SQL_LEN] to char *sql where possible

Partial (62) add load_data at the end of batch_add functions so as data can be reused by
other functions during database creation: races, genders, e3d, season, objects

Partial (71) combine add_db functions into batch_add functions wherever possible
races genders e3ds character_types, season, objects

Done (72) implement struct wrapper so as this can record if required data has been loaded
from database (required during database creation): races genders game_data channels
character_types maps map_objects seasons objects

Dropped (64) transfer send_guild_details function in guild.c to db_guild_tbl: this
function has now been changed so that it doesn't directly interface with the database

Done (58) Added command line options to reload attributes from text file

Done reordered command line options

Done (70) get_db_guild_member_list and update_db_map_objects functions now bind data
to the sql statement rather than using sprintf

Done map objects are now loaded through a separate batch_add function rather than as
part of the batch_add_maps function

Partial (68) added #notes field to lst files: character.lst

Done (72) NPC's added from char list

***************************************************************************************************

                                        TO - DO
Ref Item
--- -------------------------------------------------------------

74. Separate module for server functions in main (to reduce lines to less than 1000)

73. add weapons etc to CHARACTER TABLE and client struct

71. combine add_db functions into batch_add functions wherever possible:
npc_action, npc_trigger

65. use sqlite3_exec to speed up loading from file:
chars, chans, guilds, npc_action, npc_trigger

62. add load_data function within batch add functions:
chars, chans, guilds, npc_action, npc_trigger

68. add #notes field to lst files:
attributes, races, char types, chat, e3d, game data, gender, guilds, maps, npc action
npc trigger, objects, skills

66. stop_server needs to get function module and line from calling process

57. Fix error in map jump command

55. We have three lots of identical code in main.c to close a connection.
    Try and amalgamate

50. Implement specific function to update char to database (to avoid database actions
that don't use bind)

49. Implement function callback for idle buffer

47. Implement harvest events
43. implement harvest node restrictions
42. implement seasonal changes on harvest nodes

37. check need for initial_emu field on race table
36. check need for char_count column on character type table

35. implement pickpoints, wraith and nexus/attributes

33. create bag table
21. bag_proximity (reveal and unreveal) use destroy and create in place of revised client code

32. TEST multiple guild application handling
31. TEST database upgrade
30. TEST map object upgrade

29. for NPC's introduce time trigger to sell/boat ticket purchase so that different responses
can be engineered for different times of day.

28. add npc wear item action
27. add npc sit/stand action

26. determine how to add water lilly to item dds (object 673  items27.dds in textures plant1.dds in 3d objects)
24. harvest red bush, blue bush
23. implement fish

22. replace snprintf with ssnprintf

20. need #command to withdraw application to join guild
18. #command to change guild chan join/leave notification colours
16. #command to #letter all members of a guild
15. #IG guild channel functionality
13. need #command to #letter all guild members (guild master only)
19. need #letter system to inform ppl if guild application has been approved/rejected also if guild member leaves
12. implement guild stats
9. save guild applicant list to database

14. OPS #command to #letter all chars
11. Table to separately record all drops/pick ups in db
10. Table to separately record chars leaving and joining guilds
8. document idle_buffer2.h

5. create circular buffer for receiving packets
4. need #function to describe char and what it is wearing)
3. document new database/struct relationships
2. finish char_race_stats and char_gender_stats functions in db_char_tbl.c
1. banned chars go to ban map (jail). Dead chars got to dead map (ghost and graveyard)

***************************************************************************************************/

#endif // SERVER_BUILD_DETAILS_H_INCLUDED
