#ifndef __object__
#define __object__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "mesh.h"

typedef struct _Object Object;

struct _Object {
  Vec3 Position;
  Quat Orientation;
  Matrix4 Matrix;
  //components array
  //Mesh component
  //Box component
  Mesh* mesh;
};

void object_init(Object* o);
void object_destroy(Object* o);
void object_draw(Object* o, Evas_GL_API* gl);

void object_update(Object* o);
//void object_add_component(Component* c);

void object_add_component_mesh(Object* o, Mesh* m);


#endif
