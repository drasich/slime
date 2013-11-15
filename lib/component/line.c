#include "component/line.h"
#include "component/camera.h"
#include "property.h"


Line* 
create_line()
{
  Line* l = calloc(1, sizeof *l);
  //TODO name, shader
  l->vertices = eina_inarray_new(sizeof(GLfloat), 3);
  l->colors = eina_inarray_new(sizeof(GLfloat), 4);
  l->use_depth = false;
  return l;
}

static void*
_create_line()
{
  return create_line();
}

static Property* 
_line_properties()
{
  Property* ps = property_set_new();

  return ps;
}

static void
_line_draw(Component* c, Matrix4 world, const Matrix4 projection)
{
  Line* l = c->data;
  line_prepare_draw(l, world, projection);
  line_draw(l);
}


ComponentDesc line_desc = {
  "line",
  _create_line,
  _line_properties,
  NULL,
  NULL,
  _line_draw,
  NULL,
  NULL,
};


void 
line_clear(Line* l)
{
  eina_inarray_free(l->vertices);
  eina_inarray_free(l->colors);

  l->vertices = eina_inarray_new(sizeof(GLfloat), 3);
  l->colors = eina_inarray_new(sizeof(GLfloat), 4);
  l->need_resend = true;
}

void
line_add(Line* l, Vec3 p1, Vec3 p2)
{
  GLfloat f;
  f = p1.x;
  eina_inarray_push(l->vertices, &f);
  f = p1.y;
  eina_inarray_push(l->vertices, &f);
  f = p1.z;
  eina_inarray_push(l->vertices, &f);

  f = p2.x;
  eina_inarray_push(l->vertices, &f);
  f = p2.y;
  eina_inarray_push(l->vertices, &f);
  f = p2.z;
  eina_inarray_push(l->vertices, &f); 

  f = 1.0f;
  int i;
  for (i = 0; i < 8; ++i) { 
    eina_inarray_push(l->colors, &f);
  }

  l->need_resend = true;
}

void
line_add_color(Line* l, Vec3 p1, Vec3 p2, Vec4 color)
{
  GLfloat f;
  f = p1.x;
  eina_inarray_push(l->vertices, &f);
  f = p1.y;
  eina_inarray_push(l->vertices, &f);
  f = p1.z;
  eina_inarray_push(l->vertices, &f);

  f = p2.x;
  eina_inarray_push(l->vertices, &f);
  f = p2.y;
  eina_inarray_push(l->vertices, &f);
  f = p2.z;
  eina_inarray_push(l->vertices, &f); 

  int i;
  for (i = 0; i < 2; ++i) { 
    f = color.x;
    eina_inarray_push(l->colors, &f);
    f = color.y;
    eina_inarray_push(l->colors, &f);
    f = color.z;
    eina_inarray_push(l->colors, &f);
    f = color.w;
    eina_inarray_push(l->colors, &f);
  }

  l->need_resend = true;
}

void
line_add_box(Line* l, AABox box, Vec4 color)
{
  printf("begin addbox : %d \n", l->vertices->len);

  //TODO
  Vec3 min = box.min;
  Vec3 max = box.max;

  Vec3 p1, p2;
  
  p1 = vec3(min.x, min.y, max.z);
  p2 = vec3(max.x, min.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, max.y, max.z);
  p2 = vec3(max.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, min.y, max.z);
  p2 = vec3(min.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, min.y, max.z);
  p2 = vec3(max.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  ////////////////

  p1 = vec3(min.x, min.y, min.z);
  p2 = vec3(max.x, min.y, min.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, max.y, min.z);
  p2 = vec3(max.x, max.y, min.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, min.y, min.z);
  p2 = vec3(min.x, max.y, min.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, min.y, min.z);
  p2 = vec3(max.x, max.y, min.z);
  line_add_color(l, p1, p2, color);

  /////////////////////////

  p1 = vec3(min.x, min.y, min.z);
  p2 = vec3(min.x, min.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, max.y, min.z);
  p2 = vec3(min.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, min.y, min.z);
  p2 = vec3(max.x, min.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, max.y, min.z);
  p2 = vec3(max.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  printf("finish addbox : %d \n", l->vertices->len);

}

void
line_init(Line* l)
{
  //TODO factorize these functions
  glGenBuffers(1, &l->buffer_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  
  glBufferData(
    GL_ARRAY_BUFFER,
    l->vertices->len * l->vertices->member_size,
    l->vertices->members,
    //l->vertices_len*sizeof(GLfloat),
    //l->vertices_gl,
    GL_DYNAMIC_DRAW);

  glGenBuffers(1, &l->buffer_colors);
  glBindBuffer(GL_ARRAY_BUFFER, l->buffer_colors);
  
  glBufferData(
    GL_ARRAY_BUFFER,
    l->colors->len * l->colors->member_size,
    l->colors->members,
    GL_DYNAMIC_DRAW);

  l->shader = create_shader("line", "shader/line.vert", "shader/line.frag");
  shader_use(l->shader);
  shader_init_attribute(l->shader, "vertex", &l->attribute_vertex);
  shader_init_attribute(l->shader, "color", &l->attribute_color);
  shader_init_uniform(l->shader, "matrix", &l->uniform_matrix);
  shader_init_uniform(l->shader, "texture", &l->uniform_texture);
  shader_init_uniform(l->shader, "resolution", &l->uniform_resolution);
  shader_init_uniform(l->shader, "use_depth", &l->uniform_use_depth);
  shader_init_uniform(l->shader, "size_fixed", &l->uniform_size_fixed);

  l->is_init = true;
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

  glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    l->vertices->len * l->vertices->member_size,
    l->vertices->members);
    //l->vertices_len*sizeof(GLfloat),
    //l->vertices_gl);
    
  glBindBuffer(GL_ARRAY_BUFFER, l->buffer_colors);
  glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    l->colors->len * l->colors->member_size,
    l->colors->members);
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
  glUniformMatrix4fv(l->uniform_matrix, 1, GL_FALSE, l->matrix);
  float width = 1200;
  float height = 400;
  glUniform2f(l->uniform_resolution, width, height);
}

void
line_set_use_depth(Line* l, bool b)
{
  l->use_depth = b;
}

void
line_prepare_draw(Line* l, Matrix4 world, const Matrix4 projection)
{
  if (!l->is_init) {
    line_init(l);
  }

  shader_use(l->shader);

  Matrix4 tm;
  mat4_multiply(projection, world, tm);
  mat4_transpose(tm, tm);
  //testing code
  /*
  if (l->fixed) {
    Vec4 zero = vec4(0,0,0,1);
    Vec4 vw = mat4_vec4_mul(tm, zero);
    double w = vw.w * 0.01;
    line_clear(l);
    AABox bb = l->boxtest;
    bb.max = vec3_mul(bb.max, w);
    bb.min = vec3_mul(bb.min, w);
    line_add_box(l, bb, vec4(0,0,1,0.2));
    line_resend(l);
  }
  */

  mat4_to_gl(tm, l->matrix);
  glUniformMatrix4fv(l->uniform_matrix, 1, GL_FALSE, l->matrix);
  float width = l->camera->width;
  float height = l->camera->height;
  glUniform2f(l->uniform_resolution, width, height);

  glUniform1i(l->uniform_use_depth, l->use_depth?1:0);
  glUniform1i(l->uniform_size_fixed, l->use_size_fixed?1:0);
}

void 
line_draw(Line* l)
{
  if (!l->is_init) {
    line_init(l);
  } else if (l->need_resend) {
    line_resend(l);
  }

  //int width = 1200;
  //int height = 400;
  
  //GLuint mypixels[width*height];    //There is no 24 bit variable, so we'll have to settle for 32 bit
  //glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8_OES, mypixels);  //No upconversion.
  //glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, mypixels);  //No upconversion.

  /*
  GLfloat mypixels[width*height];
  glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, mypixels);

  //save_png(mypixels);

	glBindTexture(GL_TEXTURE_2D, l->id_texture);
  glTexImage2D(
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
	glBindTexture(GL_TEXTURE_2D, l->id_texture);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(l->uniform_texture, 0);

  glBindBuffer(GL_ARRAY_BUFFER, l->buffer_vertices);
  glEnableVertexAttribArray(l->attribute_vertex);
  
  glVertexAttribPointer(
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

  glBindBuffer(GL_ARRAY_BUFFER, l->buffer_colors);
  glEnableVertexAttribArray(l->attribute_color);
  
  glVertexAttribPointer(
    l->attribute_color,
    4,
    GL_FLOAT,
    GL_FALSE,
    0,
    0);

  glDrawArrays(GL_LINES,0, l->vertices->len/3);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(l->attribute_vertex);
  glDisableVertexAttribArray(l->attribute_color);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
line_destroy(Line* l)
{
  if (l->is_init) {
    shader_destroy(l->shader);
    free(l->shader);

    glDeleteBuffers(1,&l->buffer_vertices);
    glDeleteBuffers(1,&l->buffer_colors);
  }

  eina_inarray_free(l->vertices);
  //free(l->vertices_gl);
  eina_inarray_free(l->colors);
  free(l);
}

void 
line_add_grid(Line* l, int num, int space)
{
  Vec4 color = vec4(1,1,1,0.1);
  Vec4 xc = vec4(1.0,0.247,0.188,0.4);
  Vec4 zc = vec4(0,0.4745,1,0.4);

  int i;
  for ( i = -num; i <= num; ++i) {
    Vec3 p1 = vec3(i*space, 0, -space*num);
    Vec3 p2 = vec3(i*space, 0, space*num);
    if (i == 0)
    line_add_color(l, p1, p2, zc);
    else
    line_add_color(l, p1, p2, color);
  }

  for ( i = -num; i <= num; ++i) {
    Vec3 p1 = vec3(-space*num, 0, i*space);
    Vec3 p2 = vec3(space*num, 0, i*space);
    if (i == 0)
    line_add_color(l, p1, p2, xc);
    else
    line_add_color(l, p1, p2,color);
  }

}

void
line_set_size_fixed(Line* l, bool b)
{
  l->use_size_fixed = b;
}
