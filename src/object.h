#ifndef __object__
#define __object__
#include "vec.h"
#include "quat.h"
#include "matrix.h"

//TODO move this to component
typedef struct _Component Component;

struct _Component {

};


typedef struct _Object Object;

struct _Object {
  Vec3 Position;
  Quat Orientation;
  Matrix4 Matrix;
  //components array
  //Mesh component
  //Box component
};

void object_init(Object* o);
void object_destroy(Object* o);
void object_draw(Object* o);

void object_update(Object* o);
void object_add_component(Component* c);


#endif
