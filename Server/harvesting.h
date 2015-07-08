/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

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

#ifndef HARVESTING_H_INCLUDED
#define HARVESTING_H_INCLUDED

#define MIN_HARVEST_PROXIMITY 2

/** RESULT  : stops a char harvesting

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
void stop_harvesting(int connection);


/** RESULT  : starts a char harvesting

    RETURNS : void

    PURPOSE : code modularity

    NOTES   :
*/
void start_harvesting(int connection, int threed_object_list_pos);

#endif // HARVESTING_H_INCLUDED
