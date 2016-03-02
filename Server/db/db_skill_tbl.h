/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/

#ifndef DB_SKILL_TBL_H_INCLUDED
#define DB_SKILL_TBL_H_INCLUDED

#define SKILL_TABLE_SQL "CREATE TABLE SKILL_TABLE( \
        SKILL_ID        INTEGER PRIMARY KEY NOT NULL, \
        SKILL_TYPE_ID   INT, \
        LEVEL           INT, \
        EXP             INT)"

/** RESULT  : loads data from the skills table into the skills array

    RETURNS : void

    PURPOSE : retrieve skills data from permanent storage

    NOTES   :
**/
void load_db_skills();


/** RESULT  : loads skill data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of skill data

    NOTES   :
*/
void batch_add_skills(int skill_type_id, char *file_name);


/** RESULT  : removes a specific skill type from the database

    RETURNS : void

    PURPOSE : allows updating of skills data

    NOTES   :
*/
void delete_db_skill(int skill_type_id);

#endif // DB_SKILLS_H_INCLUDED
