#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "global.h"
#include "database.h"

struct character_table_type{
    char field_name[50];
    char field_type[50];
    char string_type[10];
};

struct character_table_type character_table[MAX_CHARACTER_TABLE_FIELDS];

int character_table_field_count;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){

   (void)(NotUsed);

   int i;

   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");

   return 0;
}

void execute_sql(char *sql){

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    //open database
    rc = sqlite3_open(DATABASE_FILE, &db);

    //kill server if database doesn't exist
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }

    //execute sql string
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    //kill server if sql doesn't execute correctly
    if( rc != SQLITE_OK ){
        perror(sql);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    }
    else{
        fprintf(stdout, "SQL executed successfully\n");
    }

    sqlite3_close(db);
}

static int callback_query(void *data, int argc, char **argv, char **azColName){

    (void)(data);
    (void)(argc);
    (void)(azColName);

    int char_id=atoi(argv[0]);

    //printf("char_id %i\n", char_id);

    strcpy(characters.character[char_id]->char_name, argv[1]);
    strcpy(characters.character[char_id]->password, argv[2]);
    characters.character[char_id]->overall_exp=atoi(argv[3]);
    characters.character[char_id]->harvest_exp=atoi(argv[4]);

    //printf("%s\n", characters.character[char_id]->char_name);
    //printf("%s\n", characters.character[char_id]->password);
    //printf("%i\n", characters.character[char_id]->overall_exp);
    //printf("%i\n", characters.character[char_id]->harvest_exp);

    return 0;
}

void execute_sql_query(char *sql){

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    //char *sql;
    const char* data = "Callback function called";

    //Open database
    rc = sqlite3_open(DATABASE_FILE, &db);

    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    else {
      fprintf(stderr, "Opened database successfully\n");
    }

    // Create SQL statement
    //sql = "SELECT * from CHARACTER_TABLE WHERE ID='16';" ;

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback_query, (void*)data, &zErrMsg);

    if(rc!=SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    }
    else{
        fprintf(stdout, "Operation done successfully\n");
    }

    sqlite3_close(db);
}

void add_field_to_character_table(char *field_name, char *field_type, char *string_type){

    strcpy(character_table[character_table_field_count].field_name, field_name);
    strcpy(character_table[character_table_field_count].field_type, field_type);
    strcpy(character_table[character_table_field_count].string_type, string_type);

    character_table_field_count++;

    if(character_table_field_count>MAX_CHARACTER_TABLE_FIELDS){
        perror("Exceeded Max Character Table Fields");
        exit(EXIT_FAILURE);
    }
}

void construct_character_table_sql_string(char *sql){

    int i=0;

    add_field_to_character_table("ID",          "INT PRIMARY KEY NOT NULL", "%i");
    add_field_to_character_table("CHAR_NAME",   "TEXT NOT NULL",            "'%s'");
    add_field_to_character_table("PASSWORD",    "TEXT NOT NULL",            "'%s'");
    add_field_to_character_table("OVERALL_EXP", "INT NOT NULL",             "%i");
    add_field_to_character_table("HARVEST_EXP", "INT NOT NULL",             "%i");

    strcpy(sql, "CREATE TABLE CHARACTER_TABLE(");

    for(i=0; i<character_table_field_count; i++){
        sprintf(sql, "%s %s %s", sql, character_table[i].field_name, character_table[i].field_type);

        if(i<character_table_field_count-1) {
            sprintf(sql, "%s,", sql);
        }
        else {
            sprintf(sql, "%s);", sql);
        }
    }
}

void add_char_to_database(int char_id){

    int i=0;
    char sql[1024]="";
    char sql_out[1024]="";

    // Create SQL statement
    strcpy(sql,  "INSERT INTO CHARACTER_TABLE (");

    for(i=0; i<character_table_field_count; i++){
        strcat(sql, character_table[i].field_name);
        if(i<character_table_field_count-1) strcat(sql, ",");
    }

    strcat(sql, ") VALUES (");

    for(i=0; i<character_table_field_count; i++){
        strcat(sql, character_table[i].string_type);
        if(i<character_table_field_count-1) strcat(sql, ",");
    }

    strcat(sql, ");");
    sprintf(sql_out, sql,
            char_id,
            characters.character[char_id]->char_name,
            characters.character[char_id]->password,
            0,
            0);

    execute_sql(sql_out);
}

void initialise_sqlite_db(void){

    sqlite3 *db;
    int  rc;
    char sql[1024]="";

    // Open database
    rc = sqlite3_open("unoff.db", &db);

    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    else{
        fprintf(stdout, "Opened database successfully\n");
    }

    sqlite3_close(db);

    // Construct tables
    construct_character_table_sql_string(sql);
    execute_sql(sql);
}



