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
};

typedef struct _Mesh Mesh;

struct _Mesh
{
  const char* name;

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

  Eina_Array* vertexgroups;
  Eina_Inarray* vertices_base;
  //For animation I need original vertex and vertex to send
  
  AABox box;

  bool is_init;

  Eina_Inarray* buffers;
  GLenum mode;
};

Mesh* mesh_new();
void mesh_file_set(Mesh* m, const char* filename);
void mesh_init(Mesh* mesh);
void mesh_resend(Mesh* mesh);
void mesh_destroy(Mesh* m);

Buffer* mesh_buffer_get(Mesh* m, const char* name);
void mesh_buffer_add(Mesh* m, const char* name, GLenum target, const void* data, int size);

VertexGroup* mesh_find_vertexgroup(Mesh* mesh, char* name);

void create_mesh_quad(Mesh* m, int w, int h);
void quad_resize(Mesh* m, int w, int h);

#endif
