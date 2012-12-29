#ifndef __mesh__
#define __mesh__
#include "shader.h"
#include "matrix.h"
#include <stdbool.h>

typedef struct _Mesh Mesh;

struct _Mesh
{
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

  //matrices
  Matrix4GL matrix;
  Matrix3GL matrix_normal;
};


void mesh_read(Mesh* mesh, char* path);
void mesh_init(Mesh* mesh);

void mesh_set_matrix(Mesh* mesh, Matrix4 mat);

void mesh_draw(Mesh* mesh);

void mesh_init_texture(Mesh* m);

Mesh* create_mesh(char* path);

#endif
