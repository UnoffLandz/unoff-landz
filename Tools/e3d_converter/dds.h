/***
    Copyright 2014 UnoffLandz

    This file is part of e3d_conv.

    e3d_conv is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    e3d_conv is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with e3d_conv.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef DDS_H_INCLUDED
#define DDS_H_INCLUDED

#include "e3d.h"// required to access materials_hash_type2 struct

#define EXPECTED_DDS_HEADER_SIZE 128

void read_dds(char *filename, char *pf_fourcc);
void report_dds_data(int report, char *report_filename, int materials_count, struct materials_hash_type2 *materials_hash);

//int read_mipmap_data(char *filename, int report, char *report_filename); //test

#endif // DDS_H_INCLUDED
