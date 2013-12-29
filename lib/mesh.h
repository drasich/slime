#ifndef __mesh__
#define __mesh__
#include "gl.h"
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

typedef struct _VertexInfoFloat VertexInfoFloat;
struct _VertexInfoFloat
{
  Vec3f position;
  Vec3f normal;
  Vec4f color;
  Eina_Inarray* weights;
};


/*
typedef enum {
  BUFFER_VERTEX,
  BUFFER_INDEX,
  BUFFER_NORMALS,
  BUFFER_UV,
} BufferType;
*/

typedef struct _Buffer Buffer;
struct _Buffer
{
  const char* name;
  GLuint id;
  const void* data;
  int size;
  GLenum target;
  GLsizei stride;
  bool need_resend;
};

typedef struct _Mesh Mesh;

struct _Mesh
{
  const char* name;

  GLuint* indices;
  GLfloat* normals;
  GLfloat* uvs;
  GLfloat* barycentric;

  uint32_t indices_len;
  uint32_t normals_len;
  uint32_t uvs_len;
  uint32_t barycentric_len;

  bool has_uv;

  Eina_Array* vertexgroups;
  Eina_Inarray* vertices_base;
  Eina_Inarray* vertices;
  Eina_Inarray* colors;
  //For animation I need original vertex and vertex to send
  
  AABox box;

  bool is_init;

  Eina_Inarray* buffers;
  GLenum mode; //TODO in here or in component?
};

Mesh* mesh_new();
void mesh_file_set(Mesh* m, const char* filename);
void mesh_init(Mesh* mesh);
void mesh_resend(Mesh* mesh);
void mesh_destroy(Mesh* m);
void buffer_resend(Buffer* b);

Buffer* mesh_buffer_get(Mesh* m, const char* name);
void mesh_buffer_add(Mesh* m, const char* name, GLenum target, const void* data, int size);
void mesh_buffer_stride_add(Mesh* m, const char* name, GLenum target, const void* data, int size, GLsizei stride);

VertexGroup* mesh_find_vertexgroup(Mesh* mesh, char* name);

void create_mesh_quad(Mesh* m, int w, int h);
void quad_resize(Mesh* m, int w, int h);

Vec3 mesh_vertex_get(const Mesh* m, const unsigned int index);

#endif
