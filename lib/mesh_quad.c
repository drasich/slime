#include "mesh.h"

void
create_mesh_quad(Mesh* m, int w, int h)
{
  m->name = "quad";

  uint8_t nb_vert = 6;
  m->vertices = eina_inarray_new(sizeof(GLfloat), 3);

  uint8_t index;
  float hw = w*0.5f, hh = h*0.5f;
  m->box.max = vec3(hw, hh, 0);
  m->box.min = vec3(-hw, -hh, 0);

  float nhw = -hw;
  float nhh = -hh;
  float zero = 0;

#define addvert(a) eina_inarray_push(m->vertices, &a)

  addvert(nhw);
  addvert(hh);
  addvert(zero);

  addvert(hw);
  addvert(hh);
  addvert(zero);

  addvert(hw);
  addvert(nhh);
  addvert(zero);

  addvert(nhw);
  addvert(hh);
  addvert(zero);

  addvert(hw);
  addvert(nhh);
  addvert(zero);

  addvert(nhw);
  addvert(nhh);
  addvert(zero);

  mesh_buffer_add(
        m,
        "vertex",
        GL_ARRAY_BUFFER,
        m->vertices->members,
        m->vertices->len*m->vertices->member_size);

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

  mesh_buffer_add(
        m,
        "barycentric",
        GL_ARRAY_BUFFER,
        m->barycentric,
        m->barycentric_len* sizeof(GLfloat));


  m->normals = calloc(nb_vert*3, sizeof(GLfloat));
  m->normals_len = nb_vert*3;
  for (index = 0; index < nb_vert; ++index){
    m->normals[index*3] = 0;
    m->normals[index*3+1] = 0;
    m->normals[index*3+2] = 1;
  }

  mesh_buffer_add(
        m,
        "normal",
        GL_ARRAY_BUFFER,
        m->normals,
        m->normals_len* sizeof(GLfloat));

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

  mesh_buffer_add(
        m,
        "texcoord",
        GL_ARRAY_BUFFER,
        m->uvs,
        m->uvs_len* sizeof(GLfloat));

  m->mode = GL_TRIANGLES;
}
   
void
quad_resize(Mesh* m, int w, int h)
{
  float hw = w*0.5f, hh = h*0.5f;
  float nhw = -hw;
  float nhh = -hh;
  float zero = 0;

#define rep(a, b) eina_inarray_replace_at(m->vertices, a, &b)
  rep(0, nhw);
  rep(1, hh);

  rep(3, hw);
  rep(4, hh);

  rep(6, hw);
  rep(7, nhh);

  rep(9, nhw);
  rep(10, hh);

  rep(12, hw);
  rep(13, nhh);

  rep(15, nhw);
  rep(16, nhh);

  //TODO we don't need to resend normals.
  mesh_resend(m);
}
