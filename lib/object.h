#ifndef __object__
#define __object__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "mesh.h"
#include "armature.h"
#include "animation.h"
#include "component/transform.h"
#include "component.h"
#include "resource_handle.h"

typedef enum {
  ORIENTATION_QUAT,
  ORIENTATION_EULER
} OrientationType;

typedef struct _Object Object;

struct _Object {
  Transform transform;
  Vec3 position;
  Quat orientation;
  OrientationType orientation_type;
  Vec3 scale;
  Vec3 angles;
  const char* name;
  struct _Scene* scene; //TODO make a list of this?
  //Animation* animation;
  Matrix4 matrix; //computed from position and orientation
  Eina_Value data_position;
  Eina_Value data_rotation;

  //Box component
  Eina_List* components;
  Component* component;

  Object* parent;
  Eina_List* children;
  PrefabHandle prefab;

  unsigned long long id;
  bool dirty;
};

struct _Camera;

void object_init(Object* o);
void object_del(Object* o);
void object_draw(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 parent);

void object_draw_edit(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 parent);

void 
object_draw_edit2(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 world);

void object_draw_edit_component(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 parent,
      const char* name);

void 
object_draw_edit_component2(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 world,
      const char* name);

void object_update(Object* o);

Object* create_object();
Object* object_new();
Object* create_object_file(const char* file);

void object_set_position(Object* o, Vec3 v);
void object_set_orientation(Object* o, Quat q);

void object_set_pose(Object* o, char* action_name, float time);

void object_play_animation(Object* o, char* action_name);

void object_compute_matrix(Object* o, Matrix4 out);

//test
void object_compute_matrix_with_angles(Object* o, Matrix4 mat);
void object_compute_matrix_with_quat(Object* o, Matrix4 mat);
void object_compute_matrix_with_pos_quat(Object* o, Matrix4 mat, Vec3 v, Quat q);

void object_add_component(Object* o, Component* c);
void object_remove_component(Object* o, Component* c);
//TODO to remove
void object_add_component_armature(Object* o, Armature* a);

void* object_component_get(const Object* o, const char* name);

ComponentDesc* component_object_desc();
Property* property_set_object();

void object_post_read(Object* o);
void object_descriptor_delete();

void object_child_add(Object* parent, Object* child);

Vec3 object_world_position_get(Object* o);
Quat object_world_orientation_get(Object* o);
Vec3 object_world_scale_get(Object* o);

void object_world_position_set(Object* o, Vec3 worldpos);

Eina_List* object_parents_path_get(Object* o);

Object* object_copy(const Object* o);

Eina_Bool object_write(const Object* o, const char* filename);
Object* object_read(const char* filename);

Eina_List** object_components_get(const Object* o);
void object_prefab_unlink(Object* o);

Mesh* object_mesh_get(Object* o);
Armature* object_armature_get(Object* o);
Animation* object_animation_get(Object* o);

void object_components_init(Object* o);

#endif
