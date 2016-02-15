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

#ifndef ATTRIBUTES_H_INCLUDED
#define ATTRIBUTES_H_INCLUDED

#include "character_race.h"

#define MAX_ATTRIBUTES 4
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
    ATTR_NIGHT_VISION=3
};

enum{//attribute type values used in the SEND_PARTIAL_STAT command

    PHY_CUR=0,
    PHY_BASE=1,
    COO_CUR=2,
    COO_BASE=3,
    REAS_CUR=4,
    REAS_BASE=5,
    WILL_CUR=6,
    WILL_BASE=7,
    INST_CUR=8,
    INST_BASE=9,
    VIT_CUR=10,
    VIT_BASE=11,
    HUMAN_CUR=12,
    HUMAN_BASE=13,
    ANIMAL_CUR=14,
    ANIMAL_BASE=15,
    VEGETAL_CUR=16,
    VEGETAL_BASE=17,
    INORG_CUR=18,
    INORG_BASE=19,
    ARTIF_CUR=20,
    ARTIF_BASE=21,
    MAGIC_CUR=22,
    MAGIC_BASE=23,
    MAN_S_CUR=24,
    MAN_S_BASE=25,
    HARV_S_CUR=26,
    HARV_S_BASE=27,
    ALCH_S_CUR=28,
    ALCH_S_BASE=29,
    OVRL_S_CUR=30,
    OVRL_S_BASE=31,
    DEF_S_CUR=32,
    DEF_S_BASE=33,
    ATT_S_CUR=34,
    ATT_S_BASE=35,
    MAG_S_CUR=36,
    MAG_S_BASE=37,
    POT_S_CUR=38,
    POT_S_BASE=39,
    CARRY_WGHT_CUR=40,
    CARRY_WGHT_BASE=41,
    MAT_POINT_CUR=42,
    MAT_POINT_BASE=43,
    ETH_POINT_CUR=44,
    ETH_POINT_BASE=45,
    FOOD_LEV=46,
    RESEARCHING=47,
    MAG_RES=48,
    MAN_EXP=49,
    MAN_EXP_NEXT=50,
    HARV_EXP=51,
    HARV_EXP_NEXT=52,
    ALCH_EXP=53,
    ALCH_EXP_NEXT=54,
    OVRL_EXP=55,
    OVRL_EXP_NEXT=56,
    DEF_EXP=57,
    DEF_EXP_NEXT=58,
    ATT_EXP=59,
    ATT_EXP_NEXT=60,
    MAG_EXP=61,
    MAG_EXP_NEXT=62,
    POT_EXP=63,
    POT_EXP_NEXT=64,
    RESEARCH_COMPLETED=65,
    RESEARCH_TOTAL=66,
    SUM_EXP=67,
    SUM_EXP_NEXT=68,
    SUM_S_CUR=69,
    SUM_S_BASE=70,
    CRA_EXP=71,
    CRA_EXP_NEXT=72,
    CRA_S_CUR=73,
    CRA_S_BASE=74,
    ENG_EXP=75,
    ENG_EXP_NEXT=76,
    ENG_S_CUR=77,
    ENG_S_BASE=78,
    RANG_EXP=79,
    RANG_EXP_NEXT=80,
    RANG_S_CUR=81,
    RANG_S_BASE=82,
    TAIL_EXP=83,
    TAIL_EXP_NEXT=84,
    TAIL_S_CUR=85,
    TAIL_S_BASE=86,
    ACTION_POINTS_CUR=87,
    ACTION_POINTS_BASE=88
};

extern char attribute_name[MAX_ATTRIBUTES][40];

#endif // ATTRIBUTES_H_INCLUDED
