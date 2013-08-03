#ifndef __component_line__
#define __component_line__
#include "shader.h"
#include "matrix.h"
#include "geometry.h"
#include <stdbool.h>
#include "component.h"

struct _CCamera;

typedef struct _Line Line;

struct _Line
{
  char* name;
  GLuint buffer_vertices;
  GLuint buffer_colors;
  GLuint id_texture;

  GLfloat*  vertices_gl;
  uint32_t vertices_len;
  Shader* shader;

  Matrix4GL matrix;

  Eina_Inarray* vertices;
  Eina_Inarray* colors;

  GLint uniform_matrix;
  GLint uniform_texture;
  GLint uniform_resolution;
  GLint uniform_use_depth;
  GLint uniform_depth;
  GLint uniform_size_fixed;
  GLuint attribute_vertex;
  GLuint attribute_color;
  
  bool need_resend;
  bool use_perspective;
  bool use_depth;
  bool use_size_fixed;

  bool is_init;

  GLuint fbo;
  GLuint rb;
};

void line_init(Line* line);
void line_resend(Line* line);

void line_set_matrices(Line* line, Matrix4 mat, Matrix4 projection);
void line_prepare_draw(Line* l, Matrix4 mat, struct _CCamera* c);

void line_draw(Line* line);

Line* create_line();
void line_destroy(Line* l);

void line_add(Line* line, Vec3 p1, Vec3 p2);
void line_add_color(Line* line, Vec3 p1, Vec3 p2, Vec4 color);
void line_add_box(Line* line, AABox box, Vec4 color);
void line_add_grid(Line* line, int num, int space);
void line_clear(Line* line);
void line_set_use_depth(Line* l, bool b);
void line_set_use_perspective(Line* l, bool b);
void line_set_size_fixed(Line* l, bool b);

ComponentDesc line_desc;

#endif
