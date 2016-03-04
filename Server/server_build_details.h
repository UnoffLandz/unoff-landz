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
#define BUILD "11"
#define BUILD_DATE __DATE__

/***************************************************************************************************
                                    BUILD 11

Done - replaced separate database table for inventory data with binary blob in character table

Done - added command line option to load harvesting skill

Done - hard-coded server name now held on database/array

Done(34 partial) - game data, channel data, character data, guild data  now loaded from text file

Done - removed broadcast modules and moved functions to other modules

Done(40 partial) - started to convert sql statements to use parameters rather than inserted values

Done - fixed bug in HERE_YOUR_STATS protocol command

Done - removed unused character_type_name column from character type table

Done - fixed bug in exp accumulation

Done - exp and levels now saved to database

Done - increased max levels to 255

Done - improved backup file naming

Fixed - bug that resulted in starting channels not being loaded for default char

Dumped -(25) duplicated by to-do list item 35

Fixed - (44) bug resulting in black ring around chars neck

Done - (38) implement GET_CALL_INFO macro in log functions

Fixed - (45) bug resulting in player sit/stand not properly saved to database

Dumped - (41) can't use sqlite_exec with sqlite_bind, hence, the sqlite_exec can't be
used to replace prepare/step/finalize on update and insert queries

Done - replaced sqlite prepare/step/finalize with exec on process_sql function

Done - create_table function now acts as a convenience wrapper for process_sql function

***************************************************************************************************

                                TO - DO
Ref Item
--- -------------------------------------------------------------

54. On wiki, link neck attachment codes to weapons and neck attachment codes. Also, add
    explanation to neck attachment codes indicating that these are specified in xml files
    with the exception of no_neck_attachment. Also, correct the descriptions of neck items

53. Unoff-64 fails on 'Player' log in, but not on new char (need to create all new chars)

52. server crash seems to be caused by broadcasting guild event to closed socked. Need to
    update socket/actor status in close_connection function

51. prevent null database strings from causing segfaults

50. Implement specific function to update char to database

49. Implement function callback for idle buffer

48. Add char neck attachment to client array and database table

47. Implement harvest events

46. Add actor scale to client array and database table

43. implement harvest node restrictions
42. implement seasonal changes on harvest nodes

40. change sql statements to use bind parameters rather than inserts (can't do this process_sql relies on
39. implement prepare and destroy wrapper functions

37. check need for initial_emu field on race table
36. check need for char_count column on character type table

35. implement pickpoints, wraith and nexus/attributes

34. load attributes from text file

33. create bag table

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
21. bag_proximity (reveal and unreveal) use destroy and create in place of revised client code
20. need #command to withdraw application to join guild
19. need #letter system to inform ppl if guild application has been approved/rejected also if guild member leaves
18. #command to change guild chan join/leave notification colours
17. map object reserve respawn
16. #command to #letter all members of a guild
15. #IG guild channel functionality
14. OPS #command to #letter all chars
13. need #command to #letter all guild members (guild master only)
12. implement guild stats
11. Table to separately record all drops/pick ups in db
10. Table to separately record chars leaving and joining guilds
9. save guild applicant list to database
8. document idle_buffer2.h
7. convert attribute struct so as attribute type can be addressed programatically
6. fix char bobbing
5. create circular buffer for receiving packets
4. need #function to describe char and what it is wearing)
3. document new database/struct relationships
2. finish char_race_stats and char_gender_stats functions in db_char_tbl.c
1. banned chars go to ban map (jail). Dead chars got to dead map (ghost and graveyard)

***************************************************************************************************/

#endif // SERVER_BUILD_DETAILS_H_INCLUDED
