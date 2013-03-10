#include <stdio.h>
#include "mesh.h"
#include "texture.h"
#include "gl.h"
#include "read.h"

void mesh_read_file_no_indices(Mesh* mesh, FILE* f)
{
  printf("mesh_read-file no indices\n");
  mesh->name = read_name(f);

  uint16_t count;
  fread(&count, sizeof(count),1,f);
  printf("size: %d\n", count);

  float x,y,z;
  int i;

  GLfloat* vert_tmp;

  vert_tmp = calloc(count*3, sizeof(GLfloat));
  uint32_t vert_len = count*3;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    vert_tmp[i] = x;
    if (i % 3 == 0) {
      if (x > mesh->box.Max.X) mesh->box.Max.X = x;
      if (x < mesh->box.Min.X) mesh->box.Min.X = x;
    }
    else if (i % 3 == 1) {
      if (x > mesh->box.Max.Y) mesh->box.Max.Y = x;
      if (x < mesh->box.Min.Y) mesh->box.Min.Y = x;
    }
    else if (i % 3 == 2) {
      if (x > mesh->box.Max.Z) mesh->box.Max.Z = x;
      if (x < mesh->box.Min.Z) mesh->box.Min.Z = x;
    }
  }

  //printf("bounds min : %f %f %f\n", mesh->box.Min.X,mesh->box.Min.Y,mesh->box.Min.Z);
  //printf("bounds max : %4.16f %4.16f %4.16f\n", mesh->box.Max.X,mesh->box.Max.Y,mesh->box.Max.Z);

  fread(&count, sizeof(count),1,f);
  printf("faces size: %d\n", count);
  uint16_t index;
  GLuint* indices_tmp = calloc(count*3, sizeof(GLuint));
  uint32_t indices_len = count*3;

  for (i = 0; i< count*3; ++i) {
    fread(&index, 2,1,f);
    indices_tmp[i] = index;
  }

  fread(&count, sizeof(count),1,f);
  printf("normals size: %d\n", count);

  GLfloat* normals_tmp = calloc(count*3, sizeof(GLfloat));
  uint32_t normals_len = count*3;
  for (i = 0; i< count*3; ++i) {
    fread(&x, 4,1,f);
    normals_tmp[i] = x;
  }

  fread(&count, sizeof(count),1,f);
  printf("uv size: %d\n", count);
  
  mesh->has_uv = count != 0;
  uint32_t uvs_len = count*2;
  GLfloat* uvs_tmp;
  if (mesh->has_uv) {
    uvs_tmp = calloc(count*2, sizeof(GLfloat));

    for (i = 0; i< count*2; ++i) {
      fread(&x, 4,1,f);
      uvs_tmp[i] = x;
    }
  }

  mesh->vertices = calloc(indices_len*3, sizeof(GLfloat));
  mesh->vertices_len = indices_len*3;
  mesh->barycentric = calloc(indices_len*3, sizeof(GLfloat));
  mesh->barycentric_len = indices_len*3;
  mesh->vertices_base = eina_inarray_new(sizeof(VertexInfo), indices_len);
  VertexInfo vi;
  for (index = 0; index < indices_len; ++index){
    mesh->vertices[index*3] = vert_tmp[indices_tmp[index]*3];
    vi.position.X = vert_tmp[indices_tmp[index]*3];
    mesh->vertices[index*3+1] = vert_tmp[indices_tmp[index]*3+1];
    vi.position.Y = vert_tmp[indices_tmp[index]*3+1];
    mesh->vertices[index*3+2] = vert_tmp[indices_tmp[index]*3+2];
    vi.position.Z = vert_tmp[indices_tmp[index]*3+2];
    eina_inarray_push(mesh->vertices_base, &vi);

    //we are only setting 1 because the rest is already 0 with calloc
    if (index % 3 == 0)
    mesh->barycentric[index*3] = 1;
    else if (index % 3 == 1)
    mesh->barycentric[index*3 +1] = 1;
    else if (index % 3 == 2)
    mesh->barycentric[index*3 +2] = 1;
  }

  mesh->normals = calloc(indices_len*3, sizeof(GLfloat));
  mesh->normals_len = indices_len*3;
  for (index = 0; index < indices_len; ++index){
    mesh->normals[index*3] = normals_tmp[indices_tmp[index]*3];
    mesh->normals[index*3+1] = normals_tmp[indices_tmp[index]*3+1];
    mesh->normals[index*3+2] = normals_tmp[indices_tmp[index]*3+2];
  }

  if (mesh->has_uv) {
    mesh->uvs = calloc(indices_len*2, sizeof(GLfloat));
    mesh->uvs_len = indices_len*2;
    for (index = 0; index < indices_len; ++index){
      mesh->uvs[index*2] = uvs_tmp[indices_tmp[index]*2];
      mesh->uvs[index*2+1] = uvs_tmp[indices_tmp[index]*2+1];
    }
  }

  free(vert_tmp);
  free(normals_tmp);
  if (mesh->has_uv) free(uvs_tmp);

  //TODO
  //TODO from here it's vertex weight
  //TODO

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

  free(indices_tmp);

}

void
mesh_init_no_indices(Mesh* m)
{
  //TODO factorize these functions
  gl->glGenBuffers(1, &m->buffer_vertices);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->vertices_len* sizeof(GLfloat),
    m->vertices,
    GL_DYNAMIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_normals);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->normals_len* sizeof(GLfloat),
    m->normals,
    GL_DYNAMIC_DRAW);

  gl->glGenBuffers(1, &m->buffer_barycentric);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_barycentric);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->barycentric_len* sizeof(GLfloat),
    m->barycentric,
    GL_STATIC_DRAW);

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
mesh_resend_no_indices(Mesh* m)
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
mesh_draw_no_indices(Mesh* m)
{
  shader_use(m->shader);

  gl->glActiveTexture(GL_TEXTURE0);
  gl->glBindTexture(GL_TEXTURE_2D, m->id_texture);
  gl->glUniform1i(m->uniform_texture, 0);

  //texcoord
  if (m->has_uv) {
    gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
    gl->glEnableVertexAttribArray(m->attribute_texcoord);
    gl->glVertexAttribPointer(
          m->attribute_texcoord,
          2,
          GL_FLOAT,
          GL_FALSE,
          0,
          0);
  }

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glEnableVertexAttribArray(m->attribute_vertex);
  
  gl->glVertexAttribPointer(
    m->attribute_vertex,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glEnableVertexAttribArray(m->attribute_normal);
  gl->glVertexAttribPointer(
    m->attribute_normal,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_barycentric);
  gl->glEnableVertexAttribArray(m->attribute_barycentric);
  
  gl->glVertexAttribPointer(
    m->attribute_barycentric,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  gl->glDrawArrays(GL_TRIANGLES,0, m->vertices_len/3);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(m->attribute_vertex);
  gl->glDisableVertexAttribArray(m->attribute_normal);
  gl->glDisableVertexAttribArray(m->attribute_barycentric);
  
  if (m->has_uv)
  gl->glDisableVertexAttribArray(m->attribute_texcoord);
}

void
mesh_show_wireframe(Mesh* m, bool b)
{
  shader_use(m->shader);
  gl->glUniform1i(m->uniform_wireframe, b?1:0);
}

Mesh* 
create_mesh_quad(int w, int h)
{
  Mesh* m = calloc(1,sizeof(Mesh));
  m->name = "quad";

  uint8_t nb_vert = 6;
  m->vertices = calloc(nb_vert*3, sizeof(GLfloat));
  m->vertices_len = nb_vert*3;

  uint8_t index;
  float hw = w*0.5f, hh = h*0.5f;
  m->box.Max = vec3(hw, hh, 0);
  m->box.Min = vec3(-hw, -hh, 0);

  m->vertices[0] = -hw;
  m->vertices[1] = hh;
  m->vertices[2] = 0;

  m->vertices[3] = hw;
  m->vertices[4] = hh;
  m->vertices[5] = 0;

  m->vertices[6] = hw;
  m->vertices[7] = -hh;
  m->vertices[8] = 0;

  m->vertices[9] = -hw;
  m->vertices[10] = hh;
  m->vertices[11] = 0;

  m->vertices[12] = hw;
  m->vertices[13] = -hh;
  m->vertices[14] = 0;

  m->vertices[15] = -hw;
  m->vertices[16] = -hh;
  m->vertices[17] = 0;

  m->barycentric = calloc(nb_vert*3, sizeof(GLfloat));
  m->barycentric_len = nb_vert*3;
  for (index = 0; index < nb_vert; ++index){

    if (index % 3 == 0)
    m->barycentric[index*3] = 1;
    else if (index % 3 == 1)
    m->barycentric[index*3 +1] = 1;
    else if (index % 3 == 2)
    m->barycentric[index*3 +2] = 1;
  }

  m->normals = calloc(nb_vert*3, sizeof(GLfloat));
  m->normals_len = nb_vert*3;
  for (index = 0; index < nb_vert; ++index){
    m->normals[index*3] = 0;
    m->normals[index*3+1] = 0;
    m->normals[index*3+2] = 1;
  }

  m->has_uv = true;
  if (m->has_uv) {
    m->uvs = calloc(nb_vert*2, sizeof(GLfloat));
    m->uvs_len = nb_vert*2;

    m->uvs[0] = 0;
    m->uvs[1] = 0;

    m->uvs[2] = 1;
    m->uvs[3] = 0;

    m->uvs[4] = 1;
    m->uvs[5] = 1;

    m->uvs[6] = 0;
    m->uvs[7] = 0;

    m->uvs[8] = 1;
    m->uvs[9] = 1;

    m->uvs[10] = 1;
    m->uvs[11] = 0;
  }

  quad_init(m);

  return m;
}
   
void
quad_resize(Mesh* m, int w, int h)
{

  shader_use(m->shader);
  gl->glUniform2f(m->uniform_resolution, w, h);
  float hw = w*0.5f, hh = h*0.5f;

  m->vertices[0] = -hw;
  m->vertices[1] = hh;
  m->vertices[2] = 0;

  m->vertices[3] = hw;
  m->vertices[4] = hh;
  m->vertices[5] = 0;

  m->vertices[6] = hw;
  m->vertices[7] = -hh;
  m->vertices[8] = 0;

  m->vertices[9] = -hw;
  m->vertices[10] = hh;
  m->vertices[11] = 0;

  m->vertices[12] = hw;
  m->vertices[13] = -hh;
  m->vertices[14] = 0;

  m->vertices[15] = -hw;
  m->vertices[16] = -hh;
  m->vertices[17] = 0;

  //TODO we don't need to resend normals.
  mesh_resend_no_indices(m);

}

void
quad_init(Mesh* m)
{
  //TODO factorize these functions
  gl->glGenBuffers(1, &m->buffer_vertices);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->vertices_len* sizeof(GLfloat),
    m->vertices,
    GL_DYNAMIC_DRAW);

  /*
  gl->glGenBuffers(1, &m->buffer_normals);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->normals_len* sizeof(GLfloat),
    m->normals,
    GL_DYNAMIC_DRAW);
    */

  /*
  gl->glGenBuffers(1, &m->buffer_barycentric);
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_barycentric);
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    m->barycentric_len* sizeof(GLfloat),
    m->barycentric,
    GL_STATIC_DRAW);
    */

  /*
  if (m->has_uv) {
    gl->glGenBuffers(1, &m->buffer_texcoords);
    gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
    gl->glBufferData(
          GL_ARRAY_BUFFER,
          m->uvs_len* sizeof(GLfloat),
          m->uvs,
          GL_DYNAMIC_DRAW);
  }
  */
}

void
quad_draw(Mesh* m)
{
  shader_use(m->shader);

  gl->glActiveTexture(GL_TEXTURE0);
  gl->glBindTexture(GL_TEXTURE_2D, m->id_texture);
  gl->glUniform1i(m->uniform_texture, 0);

  //texcoord
  /*
  if (m->has_uv) {
    gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_texcoords);
    gl->glEnableVertexAttribArray(m->attribute_texcoord);
    gl->glVertexAttribPointer(
          m->attribute_texcoord,
          2,
          GL_FLOAT,
          GL_FALSE,
          0,
          0);
  }
  */

  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_vertices);
  gl->glEnableVertexAttribArray(m->attribute_vertex);
  
  gl->glVertexAttribPointer(
    m->attribute_vertex,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  /*
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_normals);
  gl->glEnableVertexAttribArray(m->attribute_normal);
  gl->glVertexAttribPointer(
    m->attribute_normal,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);
    */

  /*
  gl->glBindBuffer(GL_ARRAY_BUFFER, m->buffer_barycentric);
  gl->glEnableVertexAttribArray(m->attribute_barycentric);
  
  gl->glVertexAttribPointer(
    m->attribute_barycentric,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);
    */

  gl->glDrawArrays(GL_TRIANGLES,0, m->vertices_len/3);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(m->attribute_vertex);
  //gl->glDisableVertexAttribArray(m->attribute_normal);
  //gl->glDisableVertexAttribArray(m->attribute_barycentric);
  
  //if (m->has_uv)
  //gl->glDisableVertexAttribArray(m->attribute_texcoord);
}
