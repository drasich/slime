//#include <Evas_GL.h>
#include <stdio.h>
#include "mesh.h"
#include "texture.h"

void
//mesh_read(char* path, Mesh* mesh)
mesh_read(Mesh* mesh, char* path)
{
  FILE *f;
  f = fopen(path, "rb");
  fseek(f, 0, SEEK_SET);

  uint16_t count;
  fread(&count, sizeof(count),1,f);
  printf("size: %d\n", count);

  float x,y,z;
  int i;

  mesh->vertices = calloc(count*3, sizeof(GLfloat));
  mesh->vertices_len = count*3;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    mesh->vertices[i] = x;
  }

  fread(&count, sizeof(count),1,f);
  printf("faces size: %d\n", count);
  uint16_t index;
  mesh->indices = calloc(count*3, sizeof(GLuint));
  mesh->indices_len = count*3;

  for (i = 0; i< count*3; ++i) {
    fread(&index, 2,1,f);
    mesh->indices[i] = index;
  }

  fread(&count, sizeof(count),1,f);
  printf("normals size: %d\n", count);

  mesh->normals = calloc(count*3, sizeof(GLfloat));
  mesh->normals_len = count*3;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    mesh->normals[i] = x;
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

  fclose(f);

}

void
mesh_init_buffer(Mesh* m, Evas_GL_API* gl, GLenum type, GLuint* buffer)
{
}

void
mesh_init(Mesh* m, Evas_GL_API* gl)
{
  //TODO factorize these functions
  gl->glGenBuffers(1, &m->buffer_vertices);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->vertices_len* sizeof(GLfloat),
    m->vertices,
    GL_STATIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_indices);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffer_indices);
  gl->glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    m->indices_len* sizeof(GLuint),
    m->indices,
    GL_STATIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_normals);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->normals_len* sizeof(GLfloat),
    m->normals,
    GL_STATIC_DRAW);

  m->shader = malloc(sizeof(Shader));
  //TODO delete shader
  shader_init(m->shader, gl, "shader/simple.vert", "shader/simple.frag");

  mesh_init_texture(m, gl);

  if (m->has_uv) {
    gl->glGenBuffers(1, &m->buffer_texcoords);
    gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
    gl->glBufferData(
          GL_ARRAY_BUFFER,
          m->uvs_len* sizeof(GLfloat),
          m->uvs,
          GL_STATIC_DRAW);
  }

}

void
mesh_init_texture(Mesh* m, Evas_GL_API* gl)
{
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
}

void
mesh_set_matrix(Mesh* mesh, Matrix4 mat, Evas_GL_API* gl)
{
  shader_use(mesh->shader, gl);
  Matrix3 normal_mat;
  mat4_to_mat3(mat, normal_mat);
  mat3_inverse(normal_mat, normal_mat);
  mat3_to_gl(normal_mat, mesh->matrix_normal);

  //TODO remove projection from here
  Matrix4 projection;
  mat4_set_frustum(projection, -1,1,-1,1,1,1000.0f);

  Matrix4 tm;
  mat4_multiply(projection, mat, tm);
  mat4_transpose(tm, tm);
  mat4_to_gl(tm, mesh->matrix);
  gl->glUniformMatrix4fv(mesh->shader->uniform_matrix, 1, GL_FALSE, mesh->matrix);
  gl->glUniformMatrix3fv(mesh->shader->uniform_normal_matrix, 1, GL_FALSE, mesh->matrix_normal);
}

void
mesh_draw(Mesh* m, Evas_GL_API* gl)
{
  shader_use(m->shader, gl);

  gl->glActiveTexture(GL_TEXTURE0);
  gl->glBindTexture(GL_TEXTURE_2D, m->id_texture);
  gl->glUniform1i(m->shader->uniform_texture, 0);

  //texcoord
  if (m->has_uv) {
    gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
    gl->glEnableVertexAttribArray(m->shader->attribute_texcoord);
    gl->glVertexAttribPointer(
          m->shader->attribute_texcoord,
          2,
          GL_FLOAT,
          GL_FALSE,
          0,
          0);
  }

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
        m->indices_len,
        GL_UNSIGNED_INT,
        0);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(m->shader->attribute_vertex);
  gl->glDisableVertexAttribArray(m->shader->attribute_normal);
  
  if (m->has_uv)
  gl->glDisableVertexAttribArray(m->shader->attribute_texcoord);
}


Mesh* create_mesh(char* path, Evas_GL_API* gl)
{
   Mesh* m = calloc(1,sizeof(Mesh));
   mesh_read(m, path);
   mesh_init(m,gl);
   return m;
}

