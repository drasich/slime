#ifndef __resource__
#define __resource__
#include "Eina.h"
#include "component/mesh.h"
#include "shader.h"
#include "texture.h"

typedef struct _ResourceManager ResourceManager;

struct _ResourceManager {
  Eina_Hash* meshes;
  Eina_Hash* shaders;
  Eina_List* meshes_to_load;
  Eina_Hash* textures;
  Eina_List* textures_to_load;

  Eina_List* waiting;
};

Mesh* resource_mesh_get(ResourceManager* rm, const char* name);
Shader* resource_shader_get(ResourceManager* rm, const char* name);
Texture* resource_texture_get(ResourceManager* rm, const char* name);

//TODO do something like:?
void* resource_get(ResourceManager* rm, const char* group, const char* name);

void resource_read_path(ResourceManager* rm);
void resource_load(ResourceManager* rm);

void resource_simple_mesh_create(ResourceManager* rm);
void resource_shader_create(ResourceManager* rm);
void resource_texture_create(ResourceManager* rm);

Eina_Hash* resource_meshes_get(ResourceManager* rm);
Eina_Hash* resource_shaders_get(ResourceManager* rm);
Eina_Hash* resource_textures_get(ResourceManager* rm);

ResourceManager* resource_manager_create();

ResourceManager* s_rm;


//////////////////

typedef struct _Resource Resource;
struct _Resource {
  const char* name;
  int type;
  void* data;
  bool is_loaded;
};

typedef struct _ResourceHandle ResourceHandle;
struct _ResourceHandle {
  const char* name;
  Resource *resource;
};

typedef struct _TextureHandle TextureHandle;
struct _TextureHandle {
  const char* name;
  Texture *texture;
};

TextureHandle* texture_handle_new();


void resource_handle_change(ResourceManager* rm, ResourceHandle* rh, const char* name);

typedef struct _ResourceWaiting ResourceWaiting;
struct _ResourceWaiting {
  ResourceHandle* rh;
  Resource* r;
};

TextureHandle* resource_texture_handle_new(ResourceManager* rm, const char* name);
void resource_texture_handle_set(ResourceManager* rm, TextureHandle* th, const char* name);

#endif
