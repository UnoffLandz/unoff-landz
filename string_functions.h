#ifndef STRING_FUNCTIONS_H_INCLUDED
#define STRING_FUNCTIONS_H_INCLUDED

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

#endif // STRING_FUNCTIONS_H_INCLUDED
