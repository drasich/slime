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

ComponentDesc* component_line();
void linec_add_grid(LineComponent* line, int num, int space);
void linec_add_color(LineComponent* line, Vec3 p1, Vec3 p2, Vec4 color);

#endif
