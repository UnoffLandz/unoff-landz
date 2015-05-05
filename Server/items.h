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

#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED

#define MAX_ITEMS 1500

struct item_type{
    char item_name[1024];
    int harvestable; //flag that item is harvestable
    int harvest_multiplier;//amount harvested on each harvesting cycle
    int emu;
    int interval;
    int exp;
    int food_value;
    int food_cooldown;
    int organic_nexus;
    int vegetal_nexus;
};
extern struct item_type item[MAX_ITEMS];


#endif // ITEMS_H_INCLUDED
