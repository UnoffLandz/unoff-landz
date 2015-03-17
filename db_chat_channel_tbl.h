#ifndef DB_CHAT_CHANNEL_TBL_H_INCLUDED
#define DB_CHAT_CHANNEL_TBL_H_INCLUDED

#define CHANNEL_TABLE_SQL "CREATE TABLE CHANNEL_TABLE( \
        CHANNEL_ID          INTEGER PRIMARY KEY     NOT NULL, \
        TYPE                INT,  \
        OWNER_ID            INT,  \
        PASSWORD            TEXT, \
        NAME                TEXT, \
        DESCRIPTION         TEXT)"

/** RESULT  : loads data from the channel table into the channel array

    RETURNS : number of rows read from the channel table

    PURPOSE : Loads channel data from the database to memory.

    NOTES   :
**/
int load_db_channels();


/** RESULT  : adds a channel to the channel table

    RETURNS : void

    PURPOSE : a test function to add channels to the channel table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_channel(int channel_id, int owner_id, int channel_type, char *password, char *channel_name, char*channel_description);

#endif // DB_CHAT_CHANNEL_TBL_H_INCLUDED
