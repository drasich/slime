#ifndef __mesh_component__
#define __mesh_component__
#include "component.h"
#include "mesh.h"
#include "shader.h"
#include "resource.h"

typedef struct _MeshComponent MeshComponent;
struct _MeshComponent
{
  ShaderInstance* shader_instance;
  bool hide;
  MeshHandle mesh_handle;
  ShaderHandle shader_handle;
  Eina_Hash* shader_instance_hash;
};

ComponentDesc* component_mesh_desc();

Shader* mesh_component_shader_get(MeshComponent* mc);
void mesh_component_shader_set(MeshComponent* mc, Shader* s);
void mesh_component_shader_set_by_name(MeshComponent* mc, const char* name);

Mesh* mesh_component_mesh_get(const MeshComponent* mc);
void mesh_component_mesh_set(MeshComponent* mc, Mesh* m);
void mesh_component_mesh_set_by_name(MeshComponent* mc, const char* name);

#endif
