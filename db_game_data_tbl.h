#ifndef DB_GAME_DATA_TBL_H_INCLUDED
#define DB_GAME_DATA_TBL_H_INCLUDED

#define GAME_DATA_TABLE_SQL "CREATE TABLE GAME_DATA_TABLE( \
        GAME_DATA_ID        INTEGER PRIMARY KEY     NOT NULL, \
        BEAM_MAP_ID         INT, \
        BEAM_MAP_TILE       INT, \
        START_MAP_ID        INT, \
        START_MAP_TILE      INT \
        )"


/** RESULT  : loads data from the game data table into the game data array

    RETURNS : number of rows read from the game data table

    PURPOSE : Loads game data from the database to memory.

    NOTES   :
**/
int load_db_game_data();


/** RESULT  : adds game data to the game data table

    RETURNS : void

    PURPOSE : a test function to load game data to the game data table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_game_data(int beam_map_id, int beam_map_tile, int start_map_id, int start_map_tile);

#endif // DB_GAME_DATA_TBL_H_INCLUDED
