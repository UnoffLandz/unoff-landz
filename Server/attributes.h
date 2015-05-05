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

#ifndef ATTRIBUTES_H_INCLUDED
#define ATTRIBUTES_H_INCLUDED

#include "character_race.h"

#define MAX_PICKPOINTS 50

struct attribute_ {

    int night_vision[MAX_PICKPOINTS];
    int day_vision[MAX_PICKPOINTS];
    int carry_capacity[MAX_PICKPOINTS];
};
extern struct attribute_ attribute[MAX_RACES];
enum{

    ATTR_CARRY_CAPACITY=1,
    ATTR_DAY_VISION=2,
    ATTR_NIGHT_VISION=3,
};

#endif // ATTRIBUTES_H_INCLUDED
