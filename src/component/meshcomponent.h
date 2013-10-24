#ifndef __mesh_component__
#define __mesh_component__
#include "component.h"
#include "mesh.h"
#include "shader.h"

typedef struct _MeshComponent MeshComponent;
struct _MeshComponent
{
  const char* mesh_name;
  const char* shader_name;
  Mesh* mesh;
  Shader* shader;
  ShaderInstance* shader_instance;
  bool hide;
};

ComponentDesc mesh_desc;
void mesh_component_shader_set(MeshComponent* mc, Shader* s);
void mesh_component_shader_set_by_name(MeshComponent* mc, const char* name);

#endif
