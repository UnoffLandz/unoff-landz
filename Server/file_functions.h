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

#ifndef FILE_FUNCTIONS_H_INCLUDED
#define FILE_FUNCTIONS_H_INCLUDED

#include <stdbool.h>

/** RESULT  : gets the size of a file

    RETURNS : file size

    PURPOSE : supports transfer of file data to memory

    NOTES   :
**/
int get_file_size(char *file_name);


/** RESULT  : determines if a file exists

    RETURNS : TRUE/FALSE

    PURPOSE : supports detecting existing database file

    NOTES   :
**/
bool file_exists(const char *filename);

///
/// \brief Copy a file named name to newname
/// \param name - name of existing file
/// \param newname - name of resulting file
/// \return -1 on failure 0 on success
///
extern int fcopy(const char *name,const char *newname);


/** RESULT  : creates a backup of a file

    RETURNS : void

    PURPOSE : file backup

    NOTES   :
**/
void create_backup_file(const char *existing_file_name);


// TODO (themuntdregger#1#): document functions
void extract_filename_suffix(const char *str_in, char *str_out);
void extract_filename_prefix(const char *str_in, char *str_out);

#endif // FILE_FUNCTIONS_H_INCLUDED
