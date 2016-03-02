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
#include "character_skill.h"
#include "clients.h"
#include "server_protocol_functions.h"
#include "attributes.h"
#include "idle_buffer2.h"

struct skill_name_type skill_name[] = {

    {NO_SKILL, "NO SKILL"},
    {HARVESTING_SKILL, "HARVESTING"},
    {MAGIC_SKILL, "MAGIC"},
    {MANUFACTURING_SKILL, "MANUFACTURING"},
    {ALCHEMY_SKILL, "ALCHEMY"},
    {DEFENCE_SKILL, "DEFENCE"},
    {ATTACK_SKILL, "ATTACK"},
    {POTION_SKILL, "POTIONING"},
    {SUMMONING_SKILL, "SUMMONING"},
    {CRAFTING_SKILL, "CRAFTING"},
    {ENGINEERING_SKILL, "ENGINEERING"},
    {RANGING_SKILL, "RANGING"},
    {TAILORING_SKILL, "TAILORING"},
};


int get_lvl(int skill_type_id, int exp){

    /** public function - see header */

    int lvl=0;

    while(exp>skill_level[skill_type_id].max_exp[lvl] && lvl<MAX_LEVELS){

        lvl++;
    }

    return lvl;
}


void add_exp(int actor_node, int skill_id, int exp){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    switch(skill_id){

        case HARVESTING_SKILL:{

            //increment experience
            clients.client[actor_node].harvest_exp+=exp;
            send_partial_stat(socket, HARV_EXP, clients.client[actor_node].harvest_exp);

            //get current level
            int lvl=clients.client[actor_node].harvest_lvl;

            //get maximum experience for the current level
            int max_exp=skill_level[skill_id].max_exp[lvl];

            //check if we've reached the maximum experience for the current level
            if(clients.client[actor_node].harvest_exp>=max_exp){

                //increase level
                clients.client[actor_node].harvest_lvl++;

                //zero experience
                clients.client[actor_node].harvest_exp=0;

                //calculate new level and send to client
                send_partial_stat(socket,  HARV_S_CUR, clients.client[actor_node].harvest_lvl);
                send_partial_stat(socket,  HARV_S_BASE, clients.client[actor_node].harvest_lvl);//this causes the levelling effect

                //send max exp for new level to client
                send_partial_stat(socket, HARV_EXP, exp);
                send_partial_stat(socket, HARV_EXP_NEXT, skill_level[skill_id].max_exp[lvl+1]);
            }

            //update_database
            push_sql_command("UPDATE CHARACTER_TABLE SET HARVEST_EXP=%i, HARVEST_LVL=%i WHERE CHAR_ID=%i", clients.client[actor_node].harvest_exp, clients.client[actor_node].harvest_lvl, clients.client[actor_node].character_id);

            break;
        }
    }
}
