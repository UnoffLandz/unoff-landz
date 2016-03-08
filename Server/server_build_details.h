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
#define BUILD "11.1"
#define BUILD_DATE __DATE__

/***************************************************************************************************
                                    BUILD 11.1

Fixed (52) stopped guild events being broadcast to closed socked.

Fixed (53) existing sqlite database on server replaced to prevent server crash arising from legacy
           inventory data.

Fixed (51) prevented null strings from causing seg fault in strcpy

Fixed server name not being transferred to database on startup

Fixed (54) server-up time shown on console is incorrect

Fixed (56) race Description in database shows "average

Partial (40) change sql statements to use bind parameters rather than inserts
Partial (39) implement prepare and destroy wrapper functions

Done (34) load attributes from text file

Dropped (7) convert attribute struct so as attribute type can be addressed programatically

Fixed bug in command line option 'C' (create database) will not work if no existing sqlite file

***************************************************************************************************

                                        TO - DO
Ref Item
--- -------------------------------------------------------------

58. Command line options to reload attributes from text file

57. Fix error in map jump command

55. We have three lots of identical code in main.c to close a connection.
    Try and amalgamate

50. Implement specific function to update char to database

49. Implement function callback for idle buffer

48. Add char neck attachment to client array and database table

47. Implement harvest events

46. Add actor scale to client array and database table

43. implement harvest node restrictions
42. implement seasonal changes on harvest nodes

40. change sql statements to use bind parameters rather than inserts
39. implement prepare and destroy wrapper functions

37. check need for initial_emu field on race table
36. check need for char_count column on character type table

35. implement pickpoints, wraith and nexus/attributes

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
17. map object reserve respawn (duplicate 43)
16. #command to #letter all members of a guild
15. #IG guild channel functionality
14. OPS #command to #letter all chars
13. need #command to #letter all guild members (guild master only)
12. implement guild stats
11. Table to separately record all drops/pick ups in db
10. Table to separately record chars leaving and joining guilds
9. save guild applicant list to database
8. document idle_buffer2.h
6. fix char bobbing
5. create circular buffer for receiving packets
4. need #function to describe char and what it is wearing)
3. document new database/struct relationships
2. finish char_race_stats and char_gender_stats functions in db_char_tbl.c
1. banned chars go to ban map (jail). Dead chars got to dead map (ghost and graveyard)

***************************************************************************************************/

#endif // SERVER_BUILD_DETAILS_H_INCLUDED
