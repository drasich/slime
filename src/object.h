#ifndef __object__
#define __object__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "mesh.h"
#include "armature.h"

typedef struct _Object Object;

struct _Object {
  Vec3 Position;
  Quat Orientation;
  Matrix4 Matrix;
  //components array
  //Mesh component
  //Box component
  Mesh* mesh;
  Armature* armature;
  char* name;
  struct _Scene* scene; //TODO make a list of this?
};

void object_init(Object* o);
void object_destroy(Object* o);
void object_draw(Object* o, int w, int h);

void object_update(Object* o);
//void object_add_component(Component* c);

void object_add_component_mesh(Object* o, Mesh* m);
void object_add_component_armature(Object* o, Armature* a);

Object* create_object();
Object* create_object_file(const char* file);

void object_set_position(Object* o, Vec3 v);
void object_set_orientation(Object* o, Quat q);

#endif
