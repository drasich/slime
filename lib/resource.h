#ifndef __resource__
#define __resource__
#include "Eina.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "scene.h"
#include "prefab.h"

typedef struct _ResourceManager ResourceManager;

struct _ResourceManager {
  Eina_Hash* meshes;
  Eina_Hash* shaders;
  Eina_List* meshes_to_load;
  Eina_Hash* textures;
  Eina_List* images_to_load;
  Eina_List* shaders_to_load;

  Eina_Hash* scenes;
  Eina_List* scenes_to_load;

  Eina_Hash* prefabs;
  Eina_Hash* armatures;
  Eina_List* armatures_to_load;

  Eina_List* waiting;
};

Mesh* resource_mesh_get(ResourceManager* rm, const char* name);
Shader* resource_shader_get(ResourceManager* rm, const char* name);
Texture* resource_texture_get(ResourceManager* rm, const char* name);
Scene* resource_scene_get(ResourceManager* rm, const char* name);
Prefab* resource_prefab_get(ResourceManager* rm, const char* name);
Armature* resource_armature_get(ResourceManager* rm, const char* name);

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
Eina_Hash* resource_scenes_get(ResourceManager* rm);
Eina_Hash* resource_prefabs_get(ResourceManager* rm);
Eina_Hash* resource_armatures_get(ResourceManager* rm);

bool resource_scene_add(ResourceManager* rm, Scene* s);
bool resource_prefab_add(ResourceManager* rm, Prefab* p);

ResourceManager* resource_manager_create();

ResourceManager* s_rm;


void resource_scene_del(ResourceManager* rm, Scene* s);

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

void resource_armature_handle_set(ResourceManager* rm, ArmatureHandle* ah, const char* name);


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

void resource_scene_save(const Scene* s);
void resource_scenes_save();
void resource_scenes_load();
void resource_scenes_clean();
void resource_prefabs_save();

void resource_shader_update(ResourceManager* rm, const char* filename);
void resource_texture_update(ResourceManager* rm, const char* filename);

#endif
