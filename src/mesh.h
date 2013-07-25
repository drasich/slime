#ifndef __mesh__
#define __mesh__
#include "shader.h"
#include "matrix.h"
#include "geometry.h"
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
  GLuint buffer_barycentric;

  GLfloat*  vertices;
  GLuint*  indices;
  GLfloat*  normals;
  GLfloat*  uvs;
  GLfloat*  barycentric;

  uint32_t vertices_len;
  uint32_t indices_len;
  uint32_t normals_len;
  uint32_t uvs_len;
  uint32_t barycentric_len;

  bool has_uv;

  Shader* shader;

  Matrix4GL matrix;
  Matrix3GL matrix_normal;

  Eina_Array* vertexgroups;
  Eina_Inarray* vertices_base;
  //For animation I need original vertex and vertex to send

  AABox box;

  GLuint attribute_vertex;
  GLuint attribute_normal;
  GLuint attribute_barycentric;
  GLuint attribute_texcoord;

  GLint uniform_matrix;
  GLint uniform_normal_matrix;
  GLint uniform_wireframe;
  GLint uniform_texture;
  GLint uniform_resolution;

  bool is_init;
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
void mesh_destroy(Mesh* m);

VertexGroup* mesh_find_vertexgroup(Mesh* mesh, char* name);


void mesh_read_file_no_indices(Mesh* mesh, FILE* f);
void mesh_init_no_indices(Mesh* mesh);
void mesh_resend_no_indices(Mesh* mesh);
void mesh_draw_no_indices(Mesh* m);
void mesh_show_wireframe(Mesh* m, bool b);

void mesh_init_attributes(Mesh* m);
void mesh_init_uniforms(Mesh* s);

void mesh_init_uniforms(Mesh* s);

Mesh* create_mesh_quad(int w, int h);
void quad_resize(Mesh* m, int w, int h);
void quad_init(Mesh* m);
void quad_draw(Mesh* m);

#endif
