#include "clients.h"
#include "character_type.h"


int get_char_race_id(int connection){

    /** public function - see header */

    int char_type_id=clients.client[connection].char_type;

    return character_type[char_type_id].race_id;
}
