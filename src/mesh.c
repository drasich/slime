#include <Elementary.h>
#include <Eio.h>
#include <Eina.h>
#include <stdio.h>
#include "mesh.h"

void
load_model(Evas_Object *gl)
{

}

static void
open_cb(void* data, Eio_File* handler, Eina_File *file)
{
  printf("file was open\n");
  size_t s = eina_file_size_get(file);
  time_t t = eina_file_mtime_get(file);

  printf("file size: %d\n", s);
  printf("file time: %d\n", t);
}

static void
error_cb(void* data, Eio_File* handler, int error)
{
  printf("there was an error\n");

}

void
readModel(char* path, Mesh* mesh)
{
  FILE *f;
  f = fopen(path, "rb");
  fseek(f, 0, SEEK_SET);

  uint16_t count;
  fread(&count, sizeof(count),1,f);
  printf("size: %d\n", count);

  float x,y,z;
  int i;

  //mesh->vertices = calloc(count*3, sizeof(GLfloat));
  mesh->vertices = eina_inarray_new (sizeof(GLfloat), count*3);
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    //mesh->vertices[i] = x;
    eina_inarray_push(mesh->vertices, &x);
  }

  fread(&count, sizeof(count),1,f);
  printf("faces size: %d\n", count);
  uint16_t index;
  //mesh->indices = calloc(count*3, sizeof(GLuint));
  mesh->indices = eina_inarray_new (sizeof(GLuint), count*3);

  for (i = 0; i< count*3; ++i) {
    fread(&index, 2,1,f);
    //mesh->indices[i] = index;
    eina_inarray_push(mesh->indices, &index);
  }

  fread(&count, sizeof(count),1,f);
  printf("normals size: %d\n", count);

  //mesh->normals = calloc(count*3, sizeof(GLfloat));
  mesh->normals = eina_inarray_new (sizeof(GLfloat), count*3);
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    //mesh->normals[i] = x;
    eina_inarray_push(mesh->normals, &x);
  }

  fread(&count, sizeof(count),1,f);
  printf("uv size: %d\n", count);
  //mesh->uvs = calloc(count*2, sizeof(GLfloat));
  mesh->uvs = eina_inarray_new (sizeof(GLfloat), count*2);

  for (i = 0; i< count*2; ++i) {
    fread(&x, 4,1,f);
    //mesh->uvs[i] = x;
    eina_inarray_push(mesh->uvs, &x);
  }

  fclose(f);

}


void
initModel(Mesh* m, Evas_GL_API* gl)
{
  gl->glGenBuffers(1, &m->buffer_vertices);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    //eina_inarray_count(m->vertices)*sizeof(GLfloat),
    m->vertices->len* m->vertices->member_size,
    m->vertices->members,
    GL_STATIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_indices);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffer_indices);
  gl->glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    eina_inarray_count(m->indices)*sizeof(GLuint),
    m->indices->members,
    GL_STATIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_normals);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    eina_inarray_count(m->normals)*sizeof(GLfloat),
    m->normals->members,
    GL_STATIC_DRAW);

 m->shader = malloc(sizeof(Shader));
 //TODO delete shader
  shader_init(m->shader, gl, "shader/simple.vert", "shader/simple.frag");
 
}

void
initTexture(Mesh* m)
{
//TODO
}

