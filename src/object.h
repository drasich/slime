#ifndef __object__
#define __object__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "mesh.h"
#include "armature.h"
#include "animation.h"

typedef struct _Object Object;

struct _Object {
  Vec3 Position;
  Quat Orientation;
  //components array
  //Mesh component
  //Box component
  Mesh* mesh;
  Armature* armature;
  const char* name;
  struct _Scene* scene; //TODO make a list of this?
  Animation* animation;
};

void object_init(Object* o);
void object_destroy(Object* o);
void object_draw(Object* o, int w, int h, Object* camera);

void object_update(Object* o);
//void object_add_component(Component* c);

void object_add_component_mesh(Object* o, Mesh* m);
void object_add_component_armature(Object* o, Armature* a);

Object* create_object();
Object* create_object_file(const char* file);

void object_set_position(Object* o, Vec3 v);
void object_set_orientation(Object* o, Quat q);

void object_set_pose(Object* o, char* action_name, float time);

void object_play_animation(Object* o, char* action_name);

#endif
