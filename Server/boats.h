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

#ifndef BOATS_H_INCLUDED
#define BOATS_H_INCLUDED

#define MAX_BOATS 10
#define BOAT_CATCH_PROXIMITY 10

struct boat_type{

    int departure_map_id;
    int departure_map_tile;

    int destination_map_id;
    int destination_map_tile;

    int travel_time;
    int arrival_time;

    char arrival_message[1024];
    char departure_message[1024];

    int boat_map_id;
    int boat_map_tile;
    int boat_payment_price;
    int boat_payment_object_id;
};
extern struct boat_type boat[MAX_BOATS];


#endif // BOATS_H_INCLUDED
