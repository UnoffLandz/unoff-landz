#ifndef OBJ_H_INCLUDED
#define OBJ_H_INCLUDED

#define MAX_OBJ_VERTEX_COUNT 1000
#define MAX_OBJ_TEXTURE_COUNT 2000
#define MAX_OBJ_NORMAL_COUNT 1000
#define MAX_OBJ_FACE_COUNT 1100
#define MAX_OBJ_MATERIAL_COUNT 10
#define OBJ_MATERIALS_NAME_LENGTH 80

#define OBJ_FILE_LINE_LENGTH 80
#define MTL_FILE_LINE_LENGTH 80

struct obj_stats_type{

    int vertex_count;
    int texture_count;
    int normals_count;
    int face_count;
    int material_count;
};

struct obj_vertex_type{

    float x;
    float y;
    float z;
};

struct obj_texture_type{

    float x;
    float y;
};

struct obj_normal_type{

    float x;
    float y;
    float z;
};

struct obj_face_type{

    int v[3];
    int t[3];
    int n[3];
    int mtl;
};

struct obj_material_type{

    char mtl_name[80];
};

char mtl_filename[OBJ_MATERIALS_NAME_LENGTH];

struct obj_stats_type obj_stats;
struct obj_vertex_type obj_vertex[MAX_OBJ_VERTEX_COUNT];
struct obj_texture_type obj_texture[MAX_OBJ_TEXTURE_COUNT];
struct obj_normal_type obj_normal[MAX_OBJ_NORMAL_COUNT];
struct obj_face_type obj_face[MAX_OBJ_FACE_COUNT];
struct obj_material_type obj_material[MAX_OBJ_MATERIAL_COUNT];

void extract_obj_data_from_e3d();
void read_obj_data(char *filename);
void create_obj_file(char *obj_filename, char *mtl_filename);
void create_mtl_file(char *mtl_filename);
void convert_e3d_to_obj_file();
void report_obj_stats();
void get_newmtl(char *mtl_filename, char *texture_filename, char *newmtl);
void check_obj_bounds(char *filename);

#endif // OBJ_H_INCLUDED
