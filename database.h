#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#define DATABASE_FILE "unoff.db"
#define MAX_CHARACTER_TABLE_FIELDS 20

void add_char_to_database(int char_id);

void initialise_sqlite_db(void);

void execute_sql_query(char *sql);

#endif // DATABASE_H_INCLUDED
