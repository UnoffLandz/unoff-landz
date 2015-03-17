#ifndef CHARACTER_TYPE_H_INCLUDED
#define CHARACTER_TYPE_H_INCLUDED

#define MAX_CHARACTER_TYPES 7

struct character_type_type{
    int race_id;
    int gender_id;
    int char_count;
};
struct character_type_type character_type[MAX_CHARACTER_TYPES];

#endif // CHARACTER_TYPE_H_INCLUDED
