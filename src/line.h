#ifndef __line__
#define __line__
#include "shader.h"
#include "matrix.h"
#include <stdbool.h>

typedef struct _Line Line;

struct _Line
{
  char* name;
  GLuint buffer_vertices;
  GLuint id_texture;

  GLfloat*  vertices_gl;
  uint32_t vertices_len;
  Shader* shader;

  Matrix4GL matrix;

  Eina_Inarray* vertices;

  GLint uniform_matrix;
  GLint uniform_texture;
  GLint uniform_resolution;
  GLuint attribute_vertex;
  
  bool need_resend;

  GLuint fbo;
  GLuint rb;
};

void line_init(Line* line);
void line_resend(Line* line);

void line_set_matrices(Line* line, Matrix4 mat, Matrix4 projection);

void line_draw(Line* line);

Line* create_line();
void line_add(Line* line, Vec3 p1, Vec3 p2);
void line_add_box(Line* line, AABox box);
void line_add_grid(Line* line, int num, int space);
void line_clear(Line* line);


#endif
