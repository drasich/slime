#ifndef __resource__
#define __resource__
#include "Eina.h"
#include "component/mesh.h"
#include "shader.h"

typedef struct _ResourceManager ResourceManager;

struct _ResourceManager {
  Eina_Hash* meshes;
  Eina_Hash* shaders;
  Eina_List* meshes_to_load;
};

Mesh* resource_mesh_get(ResourceManager* rm, const char* name);
Shader* resource_shader_get(ResourceManager* rm, const char* name);

//TODO do something like:?
void* resource_get(ResourceManager* rm, const char* group, const char* name);

void resource_read_path(ResourceManager* rm);
void resource_load(ResourceManager* rm);

void resource_simple_mesh_create(ResourceManager* rm);
void resource_shader_create(ResourceManager* rm);

Eina_Hash* resource_meshes_get(ResourceManager* rm);
Eina_Hash* resource_shaders_get(ResourceManager* rm);

ResourceManager* resource_manager_create();

ResourceManager* s_rm;

#endif
