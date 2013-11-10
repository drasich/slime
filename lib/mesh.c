#include <stdio.h>
#include "mesh.h"
#include "texture.h"
#include "gl.h"
#include "read.h"
#include "property.h"
#include "component/camera.h"
#include "resource.h" //TODO separate mesh component and mesh and put this header in the component

void mesh_read_file(Mesh* mesh, FILE* f)
{
  printf("mesh_read-file\n");
  mesh->name = read_name(f);

  uint16_t count;
  fread(&count, sizeof(count),1,f);
  printf("size: %d\n", count);

  float x,y,z;
  int i;

  mesh->vertices = calloc(count*3, sizeof(GLfloat));
  mesh->vertices_len = count*3;
  mesh->vertices_base = eina_inarray_new(sizeof(VertexInfo), count);
  VertexInfo vi;
  //Vec3 v;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    mesh->vertices[i] = x;
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
    }
  }

  mesh_buffer_add(
        mesh,
        "vertex",
        GL_ARRAY_BUFFER,
        mesh->vertices,
        mesh->vertices_len* sizeof(GLfloat));

  //printf("bounds min : %f %f %f\n", mesh->box.min.x,mesh->box.min.y,mesh->box.min.z);
  //printf("bounds max : %4.16f %4.16f %4.16f\n", mesh->box.max.x,mesh->box.max.y,mesh->box.max.z);

  fread(&count, sizeof(count),1,f);
  printf("faces size: %d\n", count);
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
  printf("normals size: %d\n", count);

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
  printf("uv size: %d\n", count);
  
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

  printf("vertex group size: %d\n", vertex_group_count);
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
    printf("ok same size\n");
  }
  else {
    printf("Size different something wrong!!!!!!!\n");
  }
  //mesh->weights = eina_inarray_new(sizeof(Weight), weights_count);
  printf("vertex weight count : %d\n", vertex_weight_count);
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

}

void
mesh_read(Mesh* mesh, const char* path)
{
  printf("come here ~~~~~~~~~~~~~~~~4444444444444\n");
  FILE *f;
  f = fopen(path, "rb");
  fseek(f, 0, SEEK_SET);

  mesh->name = read_name(f);
  mesh_read_file(mesh, f);
  fclose(f);
}

void
mesh_init(Mesh* m)
{
  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    gl->glGenBuffers(1, &b->id);
    gl->glBindBuffer(b->target, b->id);
    gl->glBufferData(
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
    gl->glBindBuffer(b->target, b->id);
    gl->glBufferSubData(
          b->target,
          0,
          b->size,
          b->data);
  }
}

void
mesh_init_texture(Mesh* m)
{
  /*
  //TODO texture path
  Texture* tex = texture_read_png_file("model/ceil.png");
  gl->glGenTextures(1, &m->id_texture);
	gl->glBindTexture(GL_TEXTURE_2D, m->id_texture);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        //GL_RGBA, //4,
        tex->internal_format,
        tex->width,
        tex->height,
        0,
        //GL_RGBA,
        tex->format,
        GL_UNSIGNED_BYTE,
        tex->data);

  free(tex->data);
  free(tex);
  */
}

void
mesh_draw(Mesh* m)
{
  /*
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffer_indices);
  gl->glDrawElements(
        GL_TRIANGLES, 
        m->indices_len,
        GL_UNSIGNED_INT,
        0);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  */
}


Mesh*
create_mesh(const char* path)
{
  printf("come here ~~~~~~~~~~~~~~~~\n");
  Mesh* m = calloc(1,sizeof(Mesh));
  mesh_read(m, path);
  mesh_init(m);
  return m;
}

Mesh*
mesh_create()
{
  Mesh* m = calloc(1,sizeof(Mesh));
  m->buffers = eina_inarray_new(sizeof(Buffer),0);
  return m;
}

Mesh*
create_mesh_file(FILE* f)
{
  printf("come here ~~~~~~~~~~~~~~~~ 22222222\n");
  Mesh* m = calloc(1,sizeof(Mesh));
  mesh_read_file(m, f);
  //mesh_init(m);
  //mesh_read_file_no_indices(m, f);
  //mesh_init_no_indices(m);
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
  printf("todo mesh destroy \n");
  //TODO clean non opengl data
  if (!m->is_init) return;

  //TODO arrays

  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    gl->glDeleteBuffers(1,&b->id);
  }
}

void
mesh_file_set(Mesh* m, const char* filename)
{
  //return;
  FILE *f;
  f = fopen(filename, "rb");
  if (!f) {
    printf("cannot open file '%s'\n", filename);
  }
  fseek(f, 0, SEEK_SET);

  const char* type = read_string(f); //type
  printf("mesh file set, type %s\n", type);
  //mesh->name = read_name(f);
  //const char* name = read_name(f);
  //printf("mesh file set name: %s\n", name);
  mesh_read_file(m, f);
  m->name = filename;
  //mesh_read_file_no_indices(mesh, f);
  fclose(f);

}

Buffer*
mesh_buffer_get(Mesh* m, const char* name)
{
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
  Buffer b;
  b.name = name;
  b.data = data;
  b.size = size;
  b.target = target;
  eina_inarray_push(m->buffers, &b);

  /*
  gl->glGenBuffers(1, &b.id);
  gl->glBindBuffer(target, b.id);
  gl->glBufferData(
    target,
    size,
    data,
    GL_DYNAMIC_DRAW);
    */
}

