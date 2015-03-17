#ifndef CHARACTER_RACES_H_INCLUDED
#define CHARACTER_RACES_H_INCLUDED

#define MAX_RACES 7

struct race_type{
    char race_name[20];
    char race_description[160];
    int char_count;
};
struct race_type race[MAX_RACES];


/** RESULT  : gets the character race

    RETURNS : the character race id

    PURPOSE :

    NOTES   :
**/
int get_char_race_id(int connection);


#endif // CHARACTER_RACES_H_INCLUDED
