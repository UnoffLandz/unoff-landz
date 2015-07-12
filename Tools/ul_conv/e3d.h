#ifndef E3D_H_INCLUDED
#define E3D_H_INCLUDED

#include <SDL/SDL_types.h>

#include "file.h"

#define MAGIC_NUMBER_LENGTH 4
#define E3D_VERSION_LENGTH 4

#define E3D_HEADER_OFFSET 28
#define E3D_VERTEX_HASH_OFFSET 68
#define E3D_VERTEX_HASH_LENGTH 14
#define E3D_INDEX_HASH_LENGTH 2
#define E3D_MATERIAL_HASH_LENGTH 172
#define E3D_MATERIALS_NAME_LENGTH 128

#define MAX_E3D_VERTEX_COUNT 1000
#define MAX_E3D_INDEX_COUNT 1300
#define MAX_E3D_MATERIALS_COUNT 10

struct e3d_header_type{

    char magic_number[MAGIC_NUMBER_LENGTH];
    unsigned char version[E3D_VERSION_LENGTH];
    unsigned char md5_hash[MD5_HASH_LENGTH];

    int header_offset;

    int vertex_count;
    int vertex_size;
    int vertex_offset;

    int index_count;
    int index_size;
    int index_offset;

    int material_count;
    int material_size;
    int material_offset;

    unsigned char options_flag;
    unsigned char format_flag;
    unsigned char reserved1;
    unsigned char reserved2;
};

struct e3d_vertex_hash_type{

    Uint16 uvx;
    Uint16 uvy;

    Uint16 n;

    Uint16 t;

    Uint16 vx;
    Uint16 vy;
    Uint16 vz;
};

struct e3d_vertex_index_type{

    Uint16 idx;
};

struct e3d_materials_list_type{

    int options;
    char name[E3D_MATERIALS_NAME_LENGTH];
    float min_x;
    float min_y;
    float min_z;
    float max_x;
    float max_y;
    float max_z;
    int min_idx;
    int max_idx;
    int idx_start;
    int idx_count;
};

struct e3d_header_type e3d_header;
struct e3d_vertex_hash_type e3d_vertex_hash[MAX_E3D_VERTEX_COUNT];
struct e3d_vertex_index_type e3d_vertex_index[MAX_E3D_INDEX_COUNT];
struct e3d_materials_list_type e3d_materials_list[MAX_E3D_MATERIALS_COUNT];

void read_e3d_header(char *filename);
void read_e3d_vertex_hash(char *filename, int vertex_hash_offset, int vertex_count);
void read_e3d_index_hash(char *filename, int index_hash_offset, int index_count);
void read_e3d_materials_hash(char *filename, int materials_offset, int materials_count);
void extract_e3d_from_obj_data();
void create_e3d_file(char *filename);
void report_e3d_data();
void convert_obj_to_e3d_file();
//int check_e3d_duplicate_vertices();
//void remove_duplicate_e3d_vertices();

#endif // E3D_H_INCLUDED
