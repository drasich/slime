#ifndef __resource_handle__
#define __resource_handle__
#include "mesh.h"
#include "texture.h"
#include "shader.h"
//#include "prefab.h"

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
    struct _Prefab* prefab;
    struct _Armature* armature;
  };
  ResourceHandleState state;
  void* data;
  resource_changed_cb cb;
};

typedef struct _ResourceHandle MeshHandle;
typedef struct _ResourceHandle TextureHandle;
typedef struct _ResourceHandle ShaderHandle;
typedef struct _ResourceHandle PrefabHandle;
typedef struct _ResourceHandle ArmatureHandle;

TextureHandle* texture_handle_new();


#endif
