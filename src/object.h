#ifndef __object__
#define __object__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component/mesh.h"
#include "armature.h"
#include "animation.h"
#include "component/transform.h"
#include "component.h"

typedef struct _Object Object;

struct _Object {
  Transform transform;
  Vec3 Position;
  Quat Orientation;
  Vec3 angles;
  const char* name;
  struct _Scene* scene; //TODO make a list of this?
  Animation* animation;
  Matrix4 matrix; //computed from position and orientation
  Eina_Value data_position;
  Eina_Value data_rotation;

  //components array
  //Mesh component
  //Box component
  Mesh* mesh;
  Armature* armature;
  Eina_List* components;
};

void object_init(Object* o);
void object_destroy(Object* o);
void object_draw(Object* o, Matrix4 world, struct _CCamera* c);
void object_draw_edit(Object* o, Matrix4 world, struct _CCamera* c);
void object_draw_edit_component(Object* o, Matrix4 world, struct _CCamera* cam, const char* name);

void object_update(Object* o);

Object* create_object();
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

ComponentDesc object_desc;
PropertySet* property_set_object();

#endif
