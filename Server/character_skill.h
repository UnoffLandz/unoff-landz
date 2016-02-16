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

#define MAX_LEVELS 127
#define MAX_SKILLS 12

struct {

    int max_exp[MAX_LEVELS];
}skill_level[MAX_SKILLS];

enum{//skill types

    HARVESTING_SKILL    =1,
    MAGIC_SKILL         =2,
    MANUFACTURING_SKILL =3,
    ALCHEMY_SKILL       =4,
    DEFENCE_SKILL       =5,
    ATTACK_SKILL        =6,
    POTION_SKILL        =7,
    SUMMONING_SKILL     =8,
    CRAFTING_SKILL      =9,
    ENGINEERING_SKILL   =10,
    RANGING_SKILL       =11,
    TAILORING_SKILL     =12,
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


#endif // CHARACTER_SKILL_H_INCLUDED
