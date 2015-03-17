#ifndef DB_CHARACTER_RACE_TBL_H_INCLUDED
#define DB_CHARACTER_RACE_TBL_H_INCLUDED

#define RACE_TABLE_SQL "CREATE TABLE RACE_TABLE( \
        RACE_ID             INTEGER PRIMARY KEY     NOT NULL, \
        RACE_NAME           TEXT, \
        RACE_DESCRIPTION    TEXT, \
        INITIAL_EMU         INT  \
        )"

/** RESULT  : loads data from the race table into the race array

    RETURNS : number of rows read from the race table

    PURPOSE : Loads race data from the database to memory.

    NOTES   :
**/
int load_db_char_races();


/** RESULT  : loads an entry to the race table

    RETURNS : void

    PURPOSE : Loads race data to the database

    NOTES   :
**/
void add_db_race(int race_id, char *race_name, char *race_description);

#endif // DB_CHARACTER_RACE_TBL_H_INCLUDED
