#ifndef STRING_FUNCTIONS_H_INCLUDED
#define STRING_FUNCTIONS_H_INCLUDED

#define ASCII_BACKSLASH 47 //used to separate file names from path
#define ASCII_SPACE 32
#define ASCII_UNDERSCORE 95 // used to remove underscores from item names
#define ASCII_HASH 35

// trims space from the right of a string
void str_trim_right(char *str_in);

// trims space from the left of a string
void str_trim_left(char *str_in);

// converts characters in a string to upper case
void str_conv_upper(char *str_in);

// converts characters in a string to lower case
void str_conv_lower(char *str_in);

// counts the number of text island in a sea of spaces
int count_str_island(char *str_in);

// gets an island from a sea of spaces
void get_str_island(char *str_in, char *str_out, int island_no);

// extracts a file name from an address
void extract_file_name(char *str_in, char *str_out);

// converts underscores to spaces
void str_remove_underscores(char *str_in);


#endif // STRING_FUNCTIONS_H_INCLUDED
