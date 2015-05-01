#include <libxml/parser.h>
#include "elfilewrapper.h"
#include "xmlcallbacks.h"
#include "../errors.h"

int el_xml_input_match(char const *file_name)
{
    return el_file_exists_anywhere (file_name);
}

void *el_xml_input_open(char const *file_name)
{
    return el_open_anywhere (file_name);
}

int el_xml_input_read(void *context, char *buffer, int len)
{
    return el_read((el_file_ptr)context, len, buffer);
}

int el_xml_input_close(void *context)
{
    el_close((el_file_ptr)context);

    return 1;
}

void xml_register_el_input_callbacks()
{
    xmlRegisterInputCallbacks(el_xml_input_match, el_xml_input_open,
        el_xml_input_read, el_xml_input_close);
}

