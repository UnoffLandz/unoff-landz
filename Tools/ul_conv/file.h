#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#define MAX_FILENAME_LENGTH 80
#define FILENAME_SUFFIX_LENGTH 5 // format = dot plus three chars plus null terminator

#define MD5_MAX_DATA 12000
#define MD5_HASH_LENGTH 16
#define MD5_TEMP_FILENAME "md5.tmp"

void get_filename_prefix(char *filename, char *prefix);

void get_filename_suffix(char *filename, char *suffix);

int get_file_size(char *filename);

void get_md5(char *filename, int offset, unsigned char *md5_hash);

#endif // FILE_H_INCLUDED
