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
#ifndef CHARACTER_SKILL_H_INCLUDED
#define CHARACTER_SKILL_H_INCLUDED

#define HARVESTING_SKILL_FILE "harvesting_skill.lst"

#define MAX_LEVELS 255
#define MAX_SKILLS 13

struct {

    int max_exp[MAX_LEVELS];
}skill_level[MAX_SKILLS+1];

enum{//skill types

    NO_SKILL,
    HARVESTING_SKILL,
    MAGIC_SKILL,
    MANUFACTURING_SKILL,
    ALCHEMY_SKILL,
    DEFENCE_SKILL,
    ATTACK_SKILL,
    POTION_SKILL,
    SUMMONING_SKILL,
    CRAFTING_SKILL,
    ENGINEERING_SKILL,
    RANGING_SKILL,
    TAILORING_SKILL
};

struct skill_name_type{

    int skill_id;
    char skill[80];
};
extern struct skill_name_type skill_name[MAX_SKILLS];


/** RESULT   : adds experience to a skill

    RETURNS  : void

    PURPOSE  :

    NOTES    :
**/
void add_exp(int actor_node, int skill_id, int exp);


/** RESULT   : gets the level for an amount of skill experience

    RETURNS  : level

    PURPOSE  :

    NOTES    :
**/
int get_lvl(int skill_type_id, int exp);


#endif // CHARACTER_SKILL_H_INCLUDED
