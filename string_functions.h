#ifndef STRING_FUNCTIONS_H_INCLUDED
#define STRING_FUNCTIONS_H_INCLUDED

#define ASCII_BACKSLASH 47 //used to separate file names from path
#define ASCII_SPACE 32
#define ASCII_UNDERSCORE 95 // used to remove underscores from item names
#define ASCII_HASH 35
#define ASCII_NULL 0


/** RESULT  : adds text and data to string

    RETURNS : void

    PURPOSE : safe snprintf

    NOTES   : stops server if overrun detected
*/
void ssnprintf(int max_len, char *str, char *fmt, ...);


/** RESULT  : trims space from the right of a string

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_trim_right(char *str_in);


/** RESULT  : trims space from the left of a string

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_trim_left(char *str_in);


/** RESULT  : convers characters in string to upper case

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_conv_upper(char *str_in);


/** RESULT  : converts characters in string to lower case

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_conv_lower(char *str_in);


/** RESULT  : counts the number of text islands in a sea of spaces

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
int count_str_island(char *str_in);


/** RESULT  : gets the text island specified by island_no from a sea of spaces

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void get_str_island(char *str_in, char *str_out, int island_no);


/** RESULT  : extracts a file name from a string containing a full path

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void extract_file_name(char *str_in, char *str_out);


/** RESULT  : converts underscores in a string to spaces

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_remove_underscores(char *str_in);

#endif // STRING_FUNCTIONS_H_INCLUDED
