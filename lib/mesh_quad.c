#include "mesh.h"

void
create_mesh_quad(Mesh* m, int w, int h)
{
  m->name = "quad";

  uint8_t nb_vert = 6;
  m->vertices = calloc(nb_vert*3, sizeof(GLfloat));
  m->vertices_len = nb_vert*3;

  uint8_t index;
  float hw = w*0.5f, hh = h*0.5f;
  m->box.max = vec3(hw, hh, 0);
  m->box.min = vec3(-hw, -hh, 0);

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

  mesh_buffer_add(
        m,
        "vertex",
        GL_ARRAY_BUFFER,
        m->vertices,
        m->vertices_len* sizeof(GLfloat));

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
}
   
void
quad_resize(Mesh* m, int w, int h)
{
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
  mesh_resend(m);
}
