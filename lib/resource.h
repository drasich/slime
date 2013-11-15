#ifndef __resource__
#define __resource__
#include "Eina.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "scene.h"

typedef struct _ResourceManager ResourceManager;

struct _ResourceManager {
  Eina_Hash* meshes;
  Eina_Hash* shaders;
  Eina_List* meshes_to_load;
  Eina_Hash* textures;
  Eina_List* textures_to_load;

  Eina_Hash* scenes;
  Eina_List* scenes_to_load;

  Eina_List* waiting;
};

Mesh* resource_mesh_get(ResourceManager* rm, const char* name);
Shader* resource_shader_get(ResourceManager* rm, const char* name);
Texture* resource_texture_get(ResourceManager* rm, const char* name);
Scene* resource_scene_get(ResourceManager* rm, const char* name);

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

/*
typedef struct _Resource Resource;
struct _Resource {
  const char* name;
  int type;
  void* data;
  bool is_loaded;
};
*/

typedef enum _ResourceHandleState ResourceHandleState;
enum _ResourceHandleState{
  RESOURCE_STATE_OK,
  RESOURCE_STATE_CHANGED,
  RESOURCE_STATE_LOADING,
};

typedef void (*resource_changed_cb)(const char* oldname, const char* newname, void* data);

typedef struct _ResourceHandle ResourceHandle;
struct _ResourceHandle {
  const char* name;
  union {
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
  };
  ResourceHandleState state;
  void* data;
  resource_changed_cb cb;
};

typedef struct _ResourceHandle MeshHandle;
typedef struct _ResourceHandle TextureHandle;
typedef struct _ResourceHandle ShaderHandle;

TextureHandle* texture_handle_new();

void resource_handle_change(ResourceManager* rm, ResourceHandle* rh, const char* name);

typedef struct _ResourceWaiting ResourceWaiting;
struct _ResourceWaiting {
  ResourceHandle* rh;
  //Resource* r;
};

TextureHandle* resource_texture_handle_new(ResourceManager* rm, const char* name);
void resource_texture_handle_set(ResourceManager* rm, TextureHandle* th, const char* name);

MeshHandle* resource_mesh_handle_new(ResourceManager* rm, const char* name);
void resource_mesh_handle_set(ResourceManager* rm, MeshHandle* mh, const char* name);

void resource_shader_handle_set(ResourceManager* rm, ShaderHandle* sh, const char* name);

#endif
