#ifndef CHARACTER_GENDER_H_INCLUDED
#define CHARACTER_GENDER_H_INCLUDED

#define MAX_GENDER 2

struct gender_type{
    char gender_name[20];
};
struct gender_type gender[MAX_GENDER];


/** RESULT  : gets the character gender

    RETURNS : the character gender id

    PURPOSE :

    NOTES   :
**/
int get_char_gender_id(int connection);

#endif // CHARACTER_GENDER_H_INCLUDED
