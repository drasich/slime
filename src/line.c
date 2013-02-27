#include "line.h"

Line* 
create_line()
{
  Line* l = calloc(1, sizeof l);
  //TODO name, shader
  l->vertices = eina_inarray_new(sizeof(GLfloat), 2);
  return l;
}

void
line_add(Line* l, Vec3 p1, Vec3 p2)
{
  GLfloat f;
  f = p1.X;
  eina_inarray_push(l->vertices, &f);
  f = p1.Y;
  eina_inarray_push(l->vertices, &f);
  f = p1.Z;
  eina_inarray_push(l->vertices, &f);

  f = p2.X;
  eina_inarray_push(l->vertices, &f);
  f = p2.Y;
  eina_inarray_push(l->vertices, &f);
  f = p2.Z;
  eina_inarray_push(l->vertices, &f);
}

void
line_add_box(Line* line, AABox box, Repere r)
{
  //TODO
}

void
line_init(Line* l)
{
  //TODO factorize these functions
  gl->glGenBuffers(1, &l->buffer_vertices);
  gl->glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  
  gl->glBufferData(
    GL_ARRAY_BUFFER,
    l->vertices->len * l->vertices->member_size,
    l->vertices->members,
    GL_DYNAMIC_DRAW);

  l->shader = malloc(sizeof(Shader));
  //TODO delete shader
  shader_init(l->shader, "shader/line.vert", "shader/line.frag");
  shader_init_attribute(l->shader, "vertex", &l->attribute_vertex);
  shader_init_uniform(l->shader, "matrix", &l->uniform_matrix);
}

void
line_resend(Line* l)
{
  gl->glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  gl->glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    l->vertices->len * l->vertices->member_size,
    l->vertices->members);
}

void
line_set_matrices(Line* l, Matrix4 mat, Matrix4 projection)
{
  shader_use(l->shader);

  Matrix4 tm;
  mat4_multiply(projection, mat, tm);
  mat4_transpose(tm, tm);
  mat4_to_gl(tm, l->matrix);
  gl->glUniformMatrix4fv(l->uniform_matrix, 1, GL_FALSE, l->matrix);
}

void 
line_draw(Line* l)
{
  line_resend(l);
  shader_use(l->shader);

  //TODO
  Matrix4 mt, mr, cam_mat, mo;
  mat4_set_translation(mt, vec3(0,0,20));
  mat4_set_rotation_quat(mr, quat_identity());
  mat4_multiply(mt, mr, cam_mat);
  mat4_inverse(cam_mat, cam_mat);

  float aspect = (float)1200/(float)400;
  Matrix4 projection;
  mat4_set_perspective(projection, M_PI/4.0, aspect ,1,1000.0f);

  Matrix4 mtt, mrr, mat;
  mat4_set_translation(mtt, vec3(0,0,0));
  mat4_set_rotation_quat(mrr, quat_identity());
  mat4_multiply(mtt, mrr, mat);

  mat4_multiply(cam_mat, mat, mo);
  line_set_matrices(l,mo, projection);
  //TODO

  gl->glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  gl->glEnableVertexAttribArray(l->attribute_vertex);
  
  gl->glVertexAttribPointer(
    l->attribute_vertex,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  /*
   // Debug, can be remove
  printf("vertices len : %d \n", l->vertices->len);
  int i = 0;
  for (i = 0; i < l->vertices->len; ++i) {
    printf("v[i] : %f \n", ((GLfloat*)l->vertices->members)[i]);
  }
  */

  gl->glDrawArrays(GL_LINES,0, l->vertices->len);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(l->attribute_vertex);
}
