#include <stdio.h>
#include "mesh.h"
#include "read.h"
#include "log.h"

static void
mesh_read_file(Mesh* mesh, FILE* f)
{
  mesh->name = read_name(f);

  uint16_t count;
  fread(&count, sizeof(count),1,f);
  EINA_LOG_DOM_DBG(log_mesh_dom, "vertices size: %d", count);

  float x,y,z;
  int i;

  mesh->vertices_base = eina_inarray_new(sizeof(VertexInfo), count);
  mesh->vertices = eina_inarray_new(sizeof(GLfloat), 3);
  VertexInfo vi;
  //Vec3 v;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    eina_inarray_push(mesh->vertices, &x);
    if (i % 3 == 0) {
      vi.position.x = x;
      if (x > mesh->box.max.x) mesh->box.max.x = x;
      if (x < mesh->box.min.x) mesh->box.min.x = x;
    }
    else if (i % 3 == 1) {
      vi.position.y = x;
      if (x > mesh->box.max.y) mesh->box.max.y = x;
      if (x < mesh->box.min.y) mesh->box.min.y = x;
    }
    else if (i % 3 == 2) {
      vi.position.z = x;
      if (x > mesh->box.max.z) mesh->box.max.z = x;
      if (x < mesh->box.min.z) mesh->box.min.z = x;
      eina_inarray_push(mesh->vertices_base, &vi);
      /*
      VertexInfoFloat vif;
      vif.position.x = (float) vi.position.x;
      vif.position.y = (float) vi.position.y;
      vif.position.z = (float) vi.position.z;
      eina_inarray_push(mesh->vertices, &vif);
      */
    }
  }

  /*
  mesh_buffer_add(
        mesh,
        "vertex",
        GL_ARRAY_BUFFER,
        mesh->vertices,
        mesh->vertices_len* sizeof(GLfloat));
  */

  //*
  mesh_buffer_stride_add(
        mesh,
        "vertex",
        GL_ARRAY_BUFFER,
        mesh->vertices->members,
        mesh->vertices->len * mesh->vertices->member_size,
        //mesh->vertices->member_size);
        0);
  //      */

  //printf("bounds min : %f %f %f\n", mesh->box.min.x,mesh->box.min.y,mesh->box.min.z);
  //printf("bounds max : %4.16f %4.16f %4.16f\n", mesh->box.max.x,mesh->box.max.y,mesh->box.max.z);

  fread(&count, sizeof(count),1,f);
  EINA_LOG_DOM_DBG(log_mesh_dom, "faces size: %d", count);
  uint16_t index;
  mesh->indices = calloc(count*3, sizeof(GLuint));
  mesh->indices_len = count*3;

  for (i = 0; i< count*3; ++i) {
    fread(&index, 2,1,f);
    mesh->indices[i] = index;
  }

  if (mesh->indices_len > 0) {
    mesh_buffer_add(
          mesh,
          "index",
          GL_ELEMENT_ARRAY_BUFFER,
          mesh->indices,
          mesh->indices_len* sizeof(GLuint));
  }

  fread(&count, sizeof(count),1,f);
  EINA_LOG_DOM_DBG(log_mesh_dom, "normals size: %d", count);

  mesh->normals = calloc(count*3, sizeof(GLfloat));
  mesh->normals_len = count*3;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    mesh->normals[i] = x;
    VertexInfo* vi = eina_inarray_nth(mesh->vertices_base, i / 3);
    if (i % 3 == 0) vi->normal.x = x;
    else if (i % 3 == 1) vi->normal.y = x;
    else if (i % 3 == 2) vi->normal.z = x;
  }

  if (mesh->normals_len > 0) {
    mesh_buffer_add(
          mesh,
          "normal",
          GL_ARRAY_BUFFER,
          mesh->normals,
          mesh->normals_len* sizeof(GLfloat));
  }

  fread(&count, sizeof(count),1,f);
  EINA_LOG_DOM_DBG(log_mesh_dom, "uv size: %d", count);
  
  mesh->has_uv = count != 0;
  mesh->uvs_len = count*2;
  if (mesh->has_uv) {
    mesh->uvs = calloc(count*2, sizeof(GLfloat));

    for (i = 0; i< count*2; ++i) {
      fread(&x, 4,1,f);
      mesh->uvs[i] = x;
    }
  }

  if (mesh->uvs_len > 0) {
    mesh_buffer_add(
          mesh,
          "texcoord",
          GL_ARRAY_BUFFER,
          mesh->uvs,
          mesh->uvs_len* sizeof(GLfloat));
  }


  uint16_t vertex_group_count = read_uint16(f);
  mesh->vertexgroups = eina_array_new(vertex_group_count);

  EINA_LOG_DOM_DBG(log_mesh_dom, "vertex group size: %d", vertex_group_count);

  for (i = 0; i < vertex_group_count; ++i) {
    VertexGroup* vg = malloc(sizeof(VertexGroup));
    char* name = read_string(f);
    vg->name = name;
    //printf("vertex group name : %s \n", name);
    uint16_t weights_count = read_uint16(f);
    //printf("vertex group weights nb : %d \n", weights_count);
    vg->weights = eina_inarray_new(sizeof(Weight), weights_count);
    int j;
    for (j = 0; j < weights_count; ++j) {
      uint16_t index = read_uint16(f);
      float weight = read_float(f);
      Weight w = { .index = index, .weight = weight};
      eina_inarray_push(vg->weights, &w);
      //printf("  index, weight : %d, %f\n", w.index, w.weight);
      //Weight* tw = eina_inarray_nth(vg->weights, j);
      //printf("     array index, weight : %d, %f\n", tw->index, tw->weight);
      
    }
    eina_array_push(mesh->vertexgroups, vg);
  }

  uint16_t vertex_weight_count = read_uint16(f);
  if (vertex_weight_count == eina_inarray_count(mesh->vertices_base)){
    EINA_LOG_DOM_DBG(log_mesh_dom, "ok same size");
  }
  else {
    EINA_LOG_DOM_ERR(log_mesh_dom, "Size different something wrong!!!!!!!");
  }
  //mesh->weights = eina_inarray_new(sizeof(Weight), weights_count);
  EINA_LOG_DOM_DBG(log_mesh_dom, "vertex weight count : %d", vertex_weight_count);

  for (i = 0; i < vertex_weight_count; ++i) {
    VertexInfo* vi = eina_inarray_nth(mesh->vertices_base, i);
    uint16_t weight_count = read_uint16(f);
    int j;
    vi->weights = eina_inarray_new(sizeof(Weight), weight_count);
    for (j = 0; j < weight_count; ++j) {
      Weight w;
      w.index = read_uint16(f);
      w.weight = read_float(f);
      eina_inarray_push(vi->weights, &w);
      //printf("vertex, index, weight : %d, %f\n", w.index, w.weight);
    }
  }

  //TODO 
  mesh->mode = GL_TRIANGLES;
}

void
mesh_init(Mesh* m)
{
  if (!m->buffers) {
    EINA_LOG_DOM_ERR(log_mesh_dom, "mesh init: there are no buffers");
    return;
  }

  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    glGenBuffers(1, &b->id);
    glBindBuffer(b->target, b->id);
    glBufferData(
          b->target,
          b->size,
          b->data,
          GL_DYNAMIC_DRAW);
  }

  m->is_init = true;
}

void
mesh_resend(Mesh* m)
{
  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    glBindBuffer(b->target, b->id);
    glBufferSubData(
          b->target,
          0,
          b->size,
          b->data);
  }
}

void
buffer_resend(Buffer* b)
{
  glBindBuffer(b->target, b->id);
  glBufferSubData(
        b->target,
        0,
        b->size,
        b->data);
  b->need_resend = false;
}


Mesh*
mesh_new()
{
  Mesh* m = calloc(1,sizeof(Mesh));
  m->buffers = eina_inarray_new(sizeof(Buffer),0);
  return m;
}

VertexGroup*
mesh_find_vertexgroup(Mesh* mesh, char* name)
{
  VertexGroup* vg;
  Eina_Array_Iterator it;
  unsigned int i;

  EINA_ARRAY_ITER_NEXT(mesh->vertexgroups, i, vg, it) {
    if (!strcmp(vg->name, name)) return vg;
  }
  return NULL;

}

void
mesh_destroy(Mesh* m)
{
  //TODO clean non opengl data
  if (!m->is_init) return;

  //TODO arrays

  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    glDeleteBuffers(1,&b->id);
  }
}

void
mesh_file_set(Mesh* m, const char* filename)
{
  FILE *f;
  f = fopen(filename, "rb");
  if (!f) {
    EINA_LOG_DOM_ERR(log_mesh_dom, "cannot open file '%s'", filename);
  }
  fseek(f, 0, SEEK_SET);

  const char* type = read_string(f);
  //printf("mesh file set, type %s\n", type);
  //mesh->name = read_name(f);
  //const char* name = read_name(f);
  //printf("mesh file set name: %s\n", name);
  mesh_read_file(m, f);
  //mesh_read_file_no_indices(mesh, f);
  fclose(f);

}

Buffer*
mesh_buffer_get(Mesh* m, const char* name)
{
  if (!m->buffers) {
    EINA_LOG_DOM_ERR(log_mesh_dom, "buffer get :there are no buffers");
    return NULL;
  }

  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    if (!strcmp(b->name, name)) {
      return b;
    }
  }

  return NULL;
}

void
mesh_buffer_add(Mesh* m, const char* name, GLenum target, const void* data, int size)
{
  mesh_buffer_stride_add(m, name, target, data, size, 0);
}

void
mesh_buffer_stride_add(Mesh* m, const char* name, GLenum target, const void* data, int size, GLsizei stride)
{
  Buffer* bb;
  EINA_INARRAY_FOREACH(m->buffers, bb) {
    if (!strcmp(name, bb->name)) {
      bb->data = data;
      bb->size = size;
      bb->target = target;
      bb->stride = stride;
      bb->need_resend = true;
      return;
    }
  }

  Buffer b;
  b.name = name;
  b.data = data;
  b.size = size;
  b.target = target;
  b.stride = stride;
  eina_inarray_push(m->buffers, &b);
}


Vec3
mesh_vertex_get(const Mesh* m, const unsigned int index)
{
  /*
  int id = m->indices[i];
  Vec3 v = { 
    m->vertices[id*3],
    m->vertices[id*3 + 1],
    m->vertices[id*3 + 2]
  };
  Vec3 v = { 
    m->vertices[index*3],
    m->vertices[index*3 + 1],
    m->vertices[index*3 + 2]
  };
  return v;
  */
  Vec3 v = ((VertexInfo*)m->vertices_base->members)[index].position;
  return v;
}


