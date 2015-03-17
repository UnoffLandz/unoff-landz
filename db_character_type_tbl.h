#ifndef DB_CHARACTER_TYPE_TBL_H_INCLUDED
#define DB_CHARACTER_TYPE_TBL_H_INCLUDED

#define CHARACTER_TYPE_TABLE_SQL "CREATE TABLE CHARACTER_TYPE_TABLE( \
        CHARACTER_TYPE_ID   INTEGER PRIMARY KEY     NOT NULL, \
        CHARACTER_TYPE_NAME TEXT, \
        RACE_ID             INT, \
        SEX_ID              INT, \
        CHAR_COUNT          INT)"

/** RESULT  : loads data from the character type table into the character type array

    RETURNS : number of rows read from the character type table

    PURPOSE : Loads character type data from the database to memory.

    NOTES   :
**/
int load_db_char_types();

/** RESULT  : loads an entry to the character type table

    RETURNS : void

    PURPOSE : Loads character type data to the database

    NOTES   :
**/
void add_db_char_type(int char_type_id, int race_id, int gender_id);


#endif // DB_CHARACTER_TYPE_TBL_H_INCLUDED
