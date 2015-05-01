#ifndef __textaliases_h__
#define __textaliases_h__


#define NUMERIC_ALIASES_FILENAME "alias.ini"

int init_text_aliases (void);
void shutdown_text_aliases (void);
int alias_command (const char *text, int len);
int unalias_command (const char *text, int len);
int aliases_command (const char *text, int len);

int process_text_alias (char *text, int len);

#endif
