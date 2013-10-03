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
  mesh->func = &mesh_generic;
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
      vi.position.X = x;
      if (x > mesh->box.Max.X) mesh->box.Max.X = x;
      if (x < mesh->box.Min.X) mesh->box.Min.X = x;
    }
    else if (i % 3 == 1) {
      vi.position.Y = x;
      if (x > mesh->box.Max.Y) mesh->box.Max.Y = x;
      if (x < mesh->box.Min.Y) mesh->box.Min.Y = x;
    }
    else if (i % 3 == 2) {
      vi.position.Z = x;
      if (x > mesh->box.Max.Z) mesh->box.Max.Z = x;
      if (x < mesh->box.Min.Z) mesh->box.Min.Z = x;
      eina_inarray_push(mesh->vertices_base, &vi);
    }
  }

  mesh_buffer_add(
        mesh,
        "vertex",
        GL_ARRAY_BUFFER,
        mesh->vertices,
        mesh->vertices_len* sizeof(GLfloat));

  //printf("bounds min : %f %f %f\n", mesh->box.Min.X,mesh->box.Min.Y,mesh->box.Min.Z);
  //printf("bounds max : %4.16f %4.16f %4.16f\n", mesh->box.Max.X,mesh->box.Max.Y,mesh->box.Max.Z);

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
    if (i % 3 == 0) vi->normal.X = x;
    else if (i % 3 == 1) vi->normal.Y = x;
    else if (i % 3 == 2) vi->normal.Z = x;
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
  //mesh_read_file_no_indices(mesh, f);
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

  /*
  //shader_use(m->shader);
  mesh_init_attributes(m);
  mesh_init_uniforms(m);
  */

  m->is_init = true;
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

/*
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
  gl->glUniformMatrix4fv(mesh->uniform_matrix, 1, GL_FALSE, mesh->matrix);
  gl->glUniformMatrix3fv(mesh->uniform_normal_matrix, 1, GL_FALSE, mesh->matrix_normal);
}
*/

/*
void
mesh_set_matrices(Mesh* mesh, Matrix4 mat, Matrix4 projection)
{
  Matrix3 normal_mat;
  mat4_to_mat3(mat, normal_mat);
  mat3_inverse(normal_mat, normal_mat);
  mat3_transpose(normal_mat, normal_mat);
  mat3_to_gl(normal_mat, mesh->matrix_normal);

  Matrix4 tm;
  mat4_multiply(projection, mat, tm);
  mat4_transpose(tm, tm);
  mat4_to_gl(tm, mesh->matrix);
  gl->glUniformMatrix4fv(mesh->uniform_matrix, 1, GL_FALSE, mesh->matrix);
  gl->glUniformMatrix3fv(mesh->uniform_normal_matrix, 1, GL_FALSE, mesh->matrix_normal);
}
*/


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

/*
void 
mesh_shader_init_uniforms(Mesh* m, Shader* s)
{
  if (!s) return;
  shader_init_uniform(s, "matrix", &m->uniform_matrix);
  if (s->has_uniform_normal_matrix)
  shader_init_uniform(s, "normal_matrix", &m->uniform_normal_matrix);
}
*/


void
mesh_destroy(Mesh* m)
{
  //TODO clean non opengl data

  if (!m->is_init) return;

  gl->glDeleteBuffers(1,&m->buffer_vertices);
  gl->glDeleteBuffers(1,&m->buffer_normals);
  
  //TODO not yet
  //gl->glDeleteBuffers(1,&m->buffer_indices);
  
  if (m->has_uv) {
    gl->glDeleteBuffers(1,&m->buffer_texcoords);
    gl->glDeleteTextures(1,&m->id_texture);
  }

  gl->glDeleteBuffers(1,&m->buffer_barycentric);

  //TODO arrays
}



static void*
_mesh_component_create()
{
  MeshComponent* m = calloc(1,sizeof *m);
  m->textures = eina_hash_string_superfast_new(NULL);
  return m;
}

static PropertySet* 
_mesh_properties()
{
  PropertySet* ps = create_property_set();

  //ADD_PROP(ps, Mesh, name, EET_T_STRING);
  ADD_PROP(ps, Mesh, name, PROPERTY_FILENAME);

  return ps;
}

static PropertySet* 
_mesh_component_properties()
{
  PropertySet* ps = create_property_set();

  ADD_RESOURCE(ps, MeshComponent, mesh_name, "mesh");
  ADD_RESOURCE(ps, MeshComponent, shader_name, "shader");
  //ADD_PROP(ps, MeshComponent, name, PROPERTY_FILENAME);

  return ps;
}

static void 
_mesh_component_draw(Component* c, Matrix4 world, Matrix4 projection)
{
  MeshComponent* mc = c->data;
  if (!mc)
  return;

  Mesh* m = mc->mesh;
  if (!m) {
    m = resource_mesh_get(s_rm, mc->mesh_name);
    if (m) mc->mesh = m;
    else return;
  }


  Shader* s = mc->shader;
  if (!s) {
    s = resource_shader_get(s_rm, mc->shader_name);
    if (s) mc->shader = s;
    else return;
  }

  shader_use(s);

  if (!m->is_init) {
    mesh_init(m);
  }

  shader_matrices_set(s, world, projection);
  shader_mesh_draw(s,mc);
  //m->func->draw(m);
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

ComponentDesc mesh_desc = {
  "mesh",
  _mesh_component_create,
  _mesh_component_properties,
  NULL,
  NULL,
  _mesh_component_draw,
  NULL,
  NULL,
};



MeshFunc mesh_generic = {
  mesh_init,
  NULL,
  mesh_draw
};


GLint
mesh_component_texture_id_get(MeshComponent* mc, const char* name)
{
  Texture* t = eina_hash_find(mc->textures, name);
  
  if (!t) return -1;

  if (t->is_fbo && t->fbo_id) {
    return *(t->fbo_id);
  }

  return -1;

}

GLint
mesh_buffer_get(Mesh* m, const char* name)
{
  Buffer* b;
  EINA_INARRAY_FOREACH(m->buffers, b) {
    if (!strcmp(b->name, name)) {
      return b->id;
    }
  }

  return -1;
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

