#ifndef __mesh__
#define __mesh__
#include "shader.h"
#include "matrix.h"
#include <stdbool.h>

typedef struct _Weight Weight;
struct _Weight
{
  uint16_t index;
  float weight;
};

typedef struct _VertexGroup VertexGroup;
struct _VertexGroup
{
  char* name;
  //Weight* weights;
  Eina_Inarray* weights;
};

typedef struct _VertexInfo VertexInfo;
struct _VertexInfo
{
  Vec3 position;
  Vec3 normal;
  Eina_Inarray* weights;
};

typedef struct _Mesh Mesh;

struct _Mesh
{
  char* name;

  GLuint buffer_vertices;
  GLuint buffer_indices;
  GLuint buffer_normals;
  GLuint id_texture;
  GLuint buffer_texcoords;

  GLfloat*  vertices;
  GLuint*  indices;
  GLfloat*  normals;
  GLfloat*  uvs;

  int vertices_len;
  int indices_len;
  int normals_len;
  int uvs_len;

  bool has_uv;

  Shader* shader;

  Matrix4GL matrix;
  Matrix3GL matrix_normal;

  Eina_Array* vertexgroups;
  Eina_Inarray* vertices_base;
  //For animation I need original vertex and vertex to send

  Vec3 bound_min;
  Vec3 bound_max;
};

void mesh_read_file(Mesh* mesh, FILE* f);
void mesh_read(Mesh* mesh, char* path);
void mesh_init(Mesh* mesh);
void mesh_resend(Mesh* mesh);

void mesh_set_matrix(Mesh* mesh, Matrix4 mat);
void mesh_set_matrices(Mesh* mesh, Matrix4 mat, Matrix4 projection);

void mesh_draw(Mesh* mesh);

void mesh_init_texture(Mesh* m);

Mesh* create_mesh(char* path);
Mesh* create_mesh_file(FILE* f);

VertexGroup* mesh_find_vertexgroup(Mesh* mesh, char* name);

#endif
