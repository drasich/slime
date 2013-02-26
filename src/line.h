#ifndef __line__
#define __line__
#include "shader.h"
#include "matrix.h"
#include <stdbool.h>
#include "intersect.h" //TODO shouldn't need this. for aabox for now, remove later

typedef struct _VertexInfo VertexInfo;
struct _VertexInfo
{
  Vec3 position;
  Vec3 normal;
  Eina_Inarray* weights;
};

typedef struct _Line Line;

struct _Line
{
  char* name;
  GLuint buffer_vertices;

  GLfloat*  vertices_gl;
  uint32_t vertices_len;
  Shader* shader;

  Matrix4GL matrix;
  Matrix3GL matrix_normal;

  Eina_Inarray* vertices;
};

void line_init(Line* line);
void line_resend(Line* line);

void line_set_matrix(Line* mesh, Matrix4 mat);
void line_set_matrices(Line* mesh, Matrix4 mat, Matrix4 projection);

void line_draw(Line* mesh);

Line* create_line();
void line_add(Line* line, Vec3 p1, Vec3 p2);
void line_add_box(Line* line, AABox box, Repere r);
void line_clear(Line* line);


#endif
