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
