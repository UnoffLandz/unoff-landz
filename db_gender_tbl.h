#ifndef DB_CHARACTER_GENDER_TBL_H_INCLUDED
#define DB_CHARACTER_GENDER_TBL_H_INCLUDED

#define GENDER_TABLE_SQL "CREATE TABLE GENDER_TABLE( \
        GENDER_ID    INTEGER PRIMARY KEY     NOT NULL, \
        GENDER_NAME  TEXT)"

/** RESULT  : loads data from the gender table into the gender array

    RETURNS : number of rows read from the gender table

    PURPOSE : Loads gender data from the database to memory.

    NOTES   :
**/
int load_db_genders();


/** RESULT  : adds a gender to the gender table

    RETURNS : void

    PURPOSE : a test function to load genders to the gender table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_gender(int gender_id, char *gender_name);

#endif // DB_CHARACTER_GENDER_TBL_H_INCLUDED
