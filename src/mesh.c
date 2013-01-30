#include <stdio.h>
#include "mesh.h"
#include "texture.h"
#include "gl.h"
#include "read.h"

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
  mesh->vertices_base = eina_inarray_new(sizeof(VertexInfo), count);
  mesh->vertices_len = count*3;
  VertexInfo vi;
  //Vec3 v;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    mesh->vertices[i] = x;
    if (i % 3 == 0) vi.position.X = x;
    else if (i % 3 == 1) vi.position.Y = x;
    else if (i % 3 == 2) {
      vi.position.Z = x;
      eina_inarray_push(mesh->vertices_base, &vi);
    }
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
    VertexInfo* vi = eina_inarray_nth(mesh->vertices_base, i / 3);
    if (i % 3 == 0) vi->normal.X = x;
    else if (i % 3 == 1) vi->normal.Y = x;
    else if (i % 3 == 2) vi->normal.Z = x;
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
      printf("vertex, index, weight : %d, %f\n", w.index, w.weight);
    }
  }

}

void
mesh_read(Mesh* mesh, char* path)
{
  FILE *f;
  f = fopen(path, "rb");
  fseek(f, 0, SEEK_SET);

  mesh->name = read_name(f);
  mesh_read_file(mesh, f);
  fclose(f);
}

void
mesh_init_buffer(Mesh* m, GLenum type, GLuint* buffer)
{
}

void
mesh_init(Mesh* m)
{
  //TODO factorize these functions
  gl->glGenBuffers(1, &m->buffer_vertices);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->vertices_len* sizeof(GLfloat),
    m->vertices,
    GL_DYNAMIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_indices);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->buffer_indices);
  gl->glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    m->indices_len* sizeof(GLuint),
    m->indices,
    GL_DYNAMIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_normals);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->normals_len* sizeof(GLfloat),
    m->normals,
    GL_DYNAMIC_DRAW);

  m->shader = malloc(sizeof(Shader));
  //TODO delete shader
  shader_init(m->shader, "shader/simple.vert", "shader/simple.frag");

  mesh_init_texture(m);

  if (m->has_uv) {
    gl->glGenBuffers(1, &m->buffer_texcoords);
    gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
    gl->glBufferData(
          GL_ARRAY_BUFFER,
          m->uvs_len* sizeof(GLfloat),
          m->uvs,
          GL_DYNAMIC_DRAW);
  }

}

void
mesh_resend(Mesh* m)
{
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    m->vertices_len* sizeof(GLfloat),
    m->vertices);

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    m->normals_len* sizeof(GLfloat),
    m->normals);
}

void
mesh_init_texture(Mesh* m)
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
mesh_set_matrix(Mesh* mesh, Matrix4 mat)
{
  shader_use(mesh->shader);
  Matrix3 normal_mat;
  mat4_to_mat3(mat, normal_mat);
  mat3_inverse(normal_mat, normal_mat);
  mat3_to_gl(normal_mat, mesh->matrix_normal);

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
mesh_set_matrices(Mesh* mesh, Matrix4 mat, Matrix4 projection)
{
  shader_use(mesh->shader);
  Matrix3 normal_mat;
  mat4_to_mat3(mat, normal_mat);
  mat3_inverse(normal_mat, normal_mat);
  mat3_to_gl(normal_mat, mesh->matrix_normal);

  Matrix4 tm;
  mat4_multiply(projection, mat, tm);
  mat4_transpose(tm, tm);
  mat4_to_gl(tm, mesh->matrix);
  gl->glUniformMatrix4fv(mesh->shader->uniform_matrix, 1, GL_FALSE, mesh->matrix);
  gl->glUniformMatrix3fv(mesh->shader->uniform_normal_matrix, 1, GL_FALSE, mesh->matrix_normal);
}


void
mesh_draw(Mesh* m)
{
  shader_use(m->shader);

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


Mesh*
create_mesh(char* path)
{
   Mesh* m = calloc(1,sizeof(Mesh));
   mesh_read(m, path);
   mesh_init(m);
   return m;
}

Mesh*
create_mesh_file(FILE* f)
{
   Mesh* m = calloc(1,sizeof(Mesh));
   mesh_read_file(m, f);
   mesh_init(m);
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

