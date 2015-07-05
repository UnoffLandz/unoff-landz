#ifndef PROCESSING_OPTIONS_H_INCLUDED
#define PROCESSING_OPTIONS_H_INCLUDED

#include <stdbool.h>

#include "file.h"

struct p_options_type{

    char filename[MAX_FILENAME_LENGTH];
    bool convert;
    bool diagnostics_only;
    bool help;
    //bool dds;
};

struct p_options_type p_options;

void parse_command_line(int argc, char *argv[]);

#endif // PROCESSING_OPTIONS_H_INCLUDED
