#include <Elementary.h>
#include <Eio.h>
#include <Eina.h>
#include <stdio.h>
#include "mesh.h"
#include "texture.h"

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
mesh_read(char* path, Mesh* mesh)
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
mesh_init(Mesh* m, Evas_GL_API* gl)
{
  //TODO factorize these functions
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

  mesh_init_texture(m, gl);

  gl->glGenBuffers(1, &m->buffer_texcoords);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
  gl->glBufferData(
         GL_ARRAY_BUFFER,
         m->uvs->len* m->uvs->member_size,
         m->uvs->members,
         GL_STATIC_DRAW);

}

void
mesh_init_texture(Mesh* m, Evas_GL_API* gl)
{
  //TODO
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
}

void
mesh_set_matrix(Mesh* mesh, Matrix4 mat, Evas_GL_API* gl)
{
  shader_use(mesh->shader, gl);
  Matrix3 normal_mat = mat4_to_mat3(mat);
  normal_mat = mat3_inverse(normal_mat);
  mesh->matrix_normal = mat3_to_gl(normal_mat);

  //TODO remove projection from here
  Matrix4 projection = mat4_frustum(-1,1,-1,1,1,100.0f);

  Matrix4 tm = mat4_multiply(projection, mat);
  tm = mat4_transpose(tm);
  mesh->matrix = mat4_to_gl(tm);
  gl->glUniformMatrix4fv(mesh->shader->uniform_matrix, 1, GL_FALSE, &(mesh->matrix.m[0]));
  gl->glUniformMatrix3fv(mesh->shader->uniform_normal_matrix, 1, GL_FALSE, &(mesh->matrix_normal.m[0]));
}

void
mesh_draw(Mesh* m, Evas_GL_API* gl)
{
  //component draw function
  //Matrix4 mat = mat4_identity();
  Matrix4 mat = mat4_translation(0,0,-10);
  mesh_set_matrix(m, mat, gl);

  // from herereal draw function
  shader_use(m->shader, gl);

  gl->glActiveTexture(GL_TEXTURE0);
  gl->glBindTexture(GL_TEXTURE_2D, m->id_texture);
  gl->glUniform1i(m->shader->uniform_texture, 0);

  //texcoord
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
  gl->glEnableVertexAttribArray(m->shader->attribute_texcoord);
  gl->glVertexAttribPointer(
    m->shader->attribute_texcoord,
    2,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glEnableVertexAttribArray(m->shader->attribute_vertex);
  
  gl->glVertexAttribPointer(
    m->shader->attribute_vertex,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glEnableVertexAttribArray(m->shader->attribute_normal);
  gl->glVertexAttribPointer(
    m->shader->attribute_normal,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffer_indices);
  gl->glDrawElements(
        GL_TRIANGLES, 
        //gl.Sizei(len(m.indices)), 
        m->indices->len,
        GL_UNSIGNED_INT,
        0);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(m->shader->attribute_vertex);
  gl->glDisableVertexAttribArray(m->shader->attribute_normal);
  gl->glDisableVertexAttribArray(m->shader->attribute_texcoord);
}

