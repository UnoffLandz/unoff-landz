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

#include "clients.h"
#include "character_race.h"
#include "items.h"
#include "character_type.h"
#include "attributes.h"

int get_max_inventory_emu(int connection){

    /** public function - see header */

    int pick_points=clients.client[connection].physique_pp;
    int race_id=get_char_race_id(connection);

    return attribute[race_id].carry_capacity[pick_points];
}

int get_inventory_emu(int connection){

    /** public function - see header */

    int i=0;
    int total_emu=0;
    int image_id=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        image_id=clients.client[connection].client_inventory[i].image_id;
        total_emu +=(clients.client[connection].client_inventory[i].amount * item[image_id].emu);
     }

    return total_emu;
}
