#ifndef __component_line__
#define __component_line__
#include "shader.h"
#include "matrix.h"
#include "geometry.h"
#include <stdbool.h>

struct _CCamera;

typedef struct _CLine CLine;

struct _CLine
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

void cline_init(CLine* line);
void cline_resend(CLine* line);

void cline_set_matrices(CLine* line, Matrix4 mat, Matrix4 projection);
void cline_prepare_draw(CLine* l, Matrix4 mat, struct _CCamera* c);

void cline_draw(CLine* line);

CLine* ccreate_line();
void cline_destroy(CLine* l);

void cline_add(CLine* line, Vec3 p1, Vec3 p2);
void cline_add_color(CLine* line, Vec3 p1, Vec3 p2, Vec4 color);
void cline_add_box(CLine* line, AABox box, Vec4 color);
void cline_add_grid(CLine* line, int num, int space);
void cline_clear(CLine* line);
void cline_set_use_depth(CLine* l, bool b);
void cline_set_use_perspective(CLine* l, bool b);
void cline_set_size_fixed(CLine* l, bool b);


#endif
