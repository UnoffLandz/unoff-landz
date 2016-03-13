/******************************************************************************************************************
	Copyright 2014}, 2015}, 2016 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation}, either version 3 of the License}, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful},
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not}, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/
#include "database_functions.h"
#include "db_sqlite_return_codes.h"

struct sqlite_return_code_type sqlite_return_code[] = {

    [SQLITE_OK]=        {"Successful result"},                              //0
    [SQLITE_ERROR]=     {"SQL error or missing database"},                  //1
    [SQLITE_INTERNAL]=  {"Internal logic error in SQLite"},                 //2
    [SQLITE_PERM]=      {"Access permission denied"},                       //3
    [SQLITE_ABORT]=     {"Callback routine requested an abort"},            //4
    [SQLITE_BUSY]=      {"The database file is locked"},                    //5
    [SQLITE_LOCKED]=    {"A table in the database is locked"},              //6
    [SQLITE_NOMEM]=     {"A malloc() failed"},                              //7
    [SQLITE_READONLY]=  {"Attempt to write a readonly database"},           //8
    [SQLITE_INTERRUPT]= {"Operation terminated by sqlite3_interrupt()"},    //9
    [SQLITE_IOERR]=     {"Some kind of disk I/O error occurred"},           //10
    [SQLITE_CORRUPT]=   {"The database disk image is malformed"},           //11
    [SQLITE_NOTFOUND]=  {"Unknown opcode in sqlite3_file_control()"},       //12
    [SQLITE_FULL]=      {"Insertion failed because database is full"},      //13
    [SQLITE_CANTOPEN]=  {"Unable to open the database file"},               //14
    [SQLITE_PROTOCOL]=  {"Database lock protocol error"},                   //15
    [SQLITE_EMPTY]=     {"Database is empty"},                              //16
    [SQLITE_SCHEMA]=    {"The database schema changed"},                    //17
    [SQLITE_TOOBIG]=    {"String or BLOB exceeds size limit"},              //18
    [SQLITE_CONSTRAINT]={"Abort due to constraint violation"},              //19
    [SQLITE_MISMATCH]=  {"Data type mismatch"},                             //20
    [SQLITE_MISUSE]=    {"Library used incorrectly"},                       //21
    [SQLITE_NOLFS]=     {"Uses OS features not supported on host"},         //22
    [SQLITE_AUTH]=      {"Authorization denied"},                           //23
    [SQLITE_FORMAT]=    {"Auxiliary database format error"},                //24
    [SQLITE_RANGE]=     {"2nd parameter to sqlite3_bind out of range"},     //25
    [SQLITE_NOTADB]=    {"File opened that is not a database file"},        //26
    [SQLITE_NOTICE]=    {"Notifications from sqlite3_log()"},               //27
    [SQLITE_WARNING]=   {"Warnings from sqlite3_log()"},                    //28
    [SQLITE_ROW]=       {"sqlite3_step() has another row ready"},           //100
    [SQLITE_DONE]=      {"sqlite3_step() has finished executing"}           //101
};
