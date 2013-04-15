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

  l->need_resend = true;
}

void
line_add_box(Line* l, AABox box)
{
  printf("begin addbox : %d \n", l->vertices->len);

  //TODO
  Vec3 min = box.Min;
  Vec3 max = box.Max;

  Vec3 p1, p2;
  
  p1 = vec3(min.X, min.Y, max.Z);
  p2 = vec3(max.X, min.Y, max.Z);
  line_add(l, p1, p2);

  p1 = vec3(min.X, max.Y, max.Z);
  p2 = vec3(max.X, max.Y, max.Z);
  line_add(l, p1, p2);

  p1 = vec3(min.X, min.Y, max.Z);
  p2 = vec3(min.X, max.Y, max.Z);
  line_add(l, p1, p2);

  p1 = vec3(max.X, min.Y, max.Z);
  p2 = vec3(max.X, max.Y, max.Z);
  line_add(l, p1, p2);

  ////////////////

  p1 = vec3(min.X, min.Y, min.Z);
  p2 = vec3(max.X, min.Y, min.Z);
  line_add(l, p1, p2);

  p1 = vec3(min.X, max.Y, min.Z);
  p2 = vec3(max.X, max.Y, min.Z);
  line_add(l, p1, p2);

  p1 = vec3(min.X, min.Y, min.Z);
  p2 = vec3(min.X, max.Y, min.Z);
  line_add(l, p1, p2);

  p1 = vec3(max.X, min.Y, min.Z);
  p2 = vec3(max.X, max.Y, min.Z);
  line_add(l, p1, p2);

  /////////////////////////

  p1 = vec3(min.X, min.Y, min.Z);
  p2 = vec3(min.X, min.Y, max.Z);
  line_add(l, p1, p2);

  p1 = vec3(min.X, max.Y, min.Z);
  p2 = vec3(min.X, max.Y, max.Z);
  line_add(l, p1, p2);

  p1 = vec3(max.X, min.Y, min.Z);
  p2 = vec3(max.X, min.Y, max.Z);
  line_add(l, p1, p2);

  p1 = vec3(max.X, max.Y, min.Z);
  p2 = vec3(max.X, max.Y, max.Z);
  line_add(l, p1, p2);

  printf("finish addbox : %d \n", l->vertices->len);

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
    GL_DYNAMIC_DRAW);

  l->shader = malloc(sizeof(Shader));
  shader_init(l->shader, "shader/line.vert", "shader/line.frag");
  shader_init_attribute(l->shader, "vertex", &l->attribute_vertex);
  shader_init_uniform(l->shader, "matrix", &l->uniform_matrix);
  shader_init_uniform(l->shader, "texture", &l->uniform_texture);
  shader_init_uniform(l->shader, "resolution", &l->uniform_resolution);

}

void
line_resend(Line* l)
{
  printf("line resend vertices len : %d \n", l->vertices->len);
  int i = 0;
  for (i = 0; i < l->vertices->len; ++i) {
    //    printf("v[i] : %f \n", ((GLfloat*)l->vertices->members)[i]);
  }

  /*
  l->vertices_len = l->vertices->len;
  GLfloat* tmp = realloc(l->vertices_gl, l->vertices_len * sizeof(GLfloat));
  
  if (tmp != NULL) {
    l->vertices_gl = tmp;
  }
  for (i = 0; i < l->vertices_len; ++i) {
    l->vertices_gl[i] = ((GLfloat*)l->vertices->members)[i];
  }
  */

  gl->glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  gl->glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    l->vertices->len * l->vertices->member_size,
    l->vertices->members);
    //l->vertices_len*sizeof(GLfloat),
    //l->vertices_gl);
  l->need_resend = false;
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
  float width = 1200;
  float height = 400;
  gl->glUniform2f(l->uniform_resolution, width, height);
}

void 
line_draw(Line* l)
{
  if (l->need_resend) {
    line_resend(l);
  }

  //int width = 1200;
  //int height = 400;
  
  //GLuint mypixels[width*height];    //There is no 24 bit variable, so we'll have to settle for 32 bit
  //gl->glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8_OES, mypixels);  //No upconversion.
  //gl->glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, mypixels);  //No upconversion.

  /*
  GLfloat mypixels[width*height];
  gl->glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, mypixels);

  //save_png(mypixels);

	gl->glBindTexture(GL_TEXTURE_2D, l->id_texture);
  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        //GL_RGBA, //4,
        GL_DEPTH_COMPONENT,
        width,
        height,
        0,
        //GL_RGBA,
        GL_DEPTH_COMPONENT,
        //GL_UNSIGNED_INT,
        GL_FLOAT,
        0);
        mypixels);
        */

  shader_use(l->shader);
	gl->glBindTexture(GL_TEXTURE_2D, l->id_texture);
  gl->glActiveTexture(GL_TEXTURE0);
  gl->glUniform1i(l->uniform_texture, 0);

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

  gl->glDrawArrays(GL_LINES,0, l->vertices->len/3);

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl->glDisableVertexAttribArray(l->attribute_vertex);
	gl->glBindTexture(GL_TEXTURE_2D, 0);
}

void
line_destroy(Line* l)
{
  shader_destroy(l->shader);
  free(l->shader);

  gl->glDeleteBuffers(1,&l->buffer_vertices);

  eina_inarray_free(l->vertices);
  //free(l->vertices_gl);
  free(l);
}

void 
line_add_grid(Line* l, int num, int space)
{
  int i;
  for ( i = -num; i <= num; ++i) {
    Vec3 p1 = vec3(i*space, 0, -space*num);
    Vec3 p2 = vec3(i*space, 0, space*num);
    line_add(l, p1, p2);
  }

  for ( i = -num; i <= num; ++i) {
    Vec3 p1 = vec3(-space*num, 0, i*space);
    Vec3 p2 = vec3(-space*num, 0, i*space);
    line_add(l, p1, p2);
  }

}

