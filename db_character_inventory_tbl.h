#ifndef DB_CHARACTER_INVENTORY_TBL_H_INCLUDED
#define DB_CHARACTER_INVENTORY_TBL_H_INCLUDED

#define INVENTORY_TABLE_SQL "CREATE TABLE INVENTORY_TABLE( \
        ID                  INTEGER PRIMARY KEY    AUTOINCREMENT, \
        CHAR_ID             INT, \
        SLOT                INT, \
        IMAGE_ID            INT, \
        AMOUNT              INT)"


#endif // DB_CHARACTER_INVENTORY_TBL_H_INCLUDED
