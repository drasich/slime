#ifndef __line_component__
#define __line_component__
#include "component.h"
#include "mesh.h"
#include "shader.h"
#include "resource.h"

typedef struct _LineComponent LineComponent;
struct _LineComponent
{
  bool hide;
  Mesh* mesh;
  ShaderHandle shader_handle;
  ShaderInstance* shader_instance;
  struct _Camera* camera;
};

LineComponent* line_component_create();
ComponentDesc* component_line();
void line_component_draw(LineComponent* lc, Matrix4 world, const Matrix4 projection);
void line_clear(LineComponent* l);


void line_add_grid(LineComponent* line, int num, int space);
void line_add_color(LineComponent* line, Vec3 p1, Vec3 p2, Vec4 color);
void line_add_box(LineComponent* line, AABox box, Vec4 color);

#endif
