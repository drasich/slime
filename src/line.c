#include "line.h"

Line* 
create_line()
{
  Line* l = calloc(1, sizeof *l);
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

  /*
  l->vertices_len = l->vertices->len;
  GLfloat* tmp = realloc(l->vertices_gl, l->vertices->len * sizeof(GLfloat));
  
  if (tmp != NULL) {
    l->vertices_gl = tmp;
  }
  int i = 0;
  for (i = 0; i < l->vertices_len; ++i) {
    l->vertices_gl[i] = ((GLfloat*)l->vertices->members)[i];
  }
  */
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
    //l->vertices_len*sizeof(GLfloat),
    //l->vertices_gl,
    GL_STREAM_DRAW);

  l->shader = malloc(sizeof(Shader));
  shader_init(l->shader, "shader/line.vert", "shader/line.frag");
  shader_init_attribute(l->shader, "vertex", &l->attribute_vertex);
  shader_init_uniform(l->shader, "matrix", &l->uniform_matrix);
}

void
line_resend(Line* l)
{
  /*
  printf("line resend vertices len : %d \n", l->vertices->len);
  int i = 0;
  for (i = 0; i < l->vertices->len; ++i) {
    printf("v[i] : %f \n", ((GLfloat*)l->vertices->members)[i]);
  }
  */

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
  //line_resend(l);
  shader_use(l->shader);

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
  //gl->glDrawArrays(GL_LINES,0, l->vertices_len);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(l->attribute_vertex);
}

void line_destroy(Line* l)
{
  shader_destroy(l->shader);
  free(l->shader);

  gl->glDeleteBuffers(1,&l->buffer_vertices);

  eina_inarray_free(l->vertices);
  //free(l->vertices_gl);
  free(l);
}
