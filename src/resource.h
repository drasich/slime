#ifndef __resource__
#define __resource__
#include "Eina.h"
#include "component/mesh.h"

typedef struct _ResourceManager ResourceManager;

struct _ResourceManager {
  Eina_Hash* meshes;
  Eina_List* meshes_to_load;
};

Mesh* resource_mesh_get(ResourceManager* rm, const char* name);
void resource_read_path(ResourceManager* rm);
void resource_load(ResourceManager* rm);

ResourceManager* resource_manager_create();

#endif
