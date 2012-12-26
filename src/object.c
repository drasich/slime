#include "object.h"
#include "gl.h"

void
object_init(Object* o)
{
  mat4_set_identity(o->Matrix);
  quat_set_identity(&o->Orientation);
  o->name = "dance";
}

void
object_destroy(Object* o)
{
  //TODO
}

void
object_draw(Object* o)
{
  Matrix4 mt, mr, mat;
  mat4_set_translation(mt, o->Position);
  mat4_set_rotation_quat(mr, o->Orientation);
  mat4_multiply(mt, mr, mat);

  if (o->mesh != NULL) {
    mesh_set_matrix(o->mesh, mat, gl);
    mesh_draw(o->mesh);
  }
}

void
object_update(Object* o)
{
  //TODO remove this code and this function anyway
  static float test = 0;
  test += 0.05f;
  Vec3 axis = {0,1,0};
  o->Orientation = quat_angle_axis(test, axis);
}

/*
void
object_add_component(Component* c)
{
  //TODO
}
*/

void object_add_component_mesh(Object* o, Mesh* m)
{
  o->mesh = m;
  
}

Object* create_object()
{
   Object* o = calloc(1, sizeof(Object));
   object_init(o);
   return o;
}

void
object_set_position(Object* o, Vec3 v)
{
  o->Position = v;
}

void 
object_set_orientation(Object* o, Quat q)
{
  o->Orientation = q;
}
