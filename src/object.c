#include "object.h"

void
object_init(Object* o)
{
  mat4_set_identity(o->Matrix);
  quat_set_identity(&o->Orientation);
}

void
object_destroy(Object* o)
{
  //TODO
}

void
object_draw(Object* o, Evas_GL_API* gl)
{
  Matrix4 mat;
  Vec3 t = {0,-5,-10};
  mat4_set_translation(mat, t);

  if (o->mesh != NULL) {
    mesh_set_matrix(o->mesh, mat, gl);
    mesh_draw(o->mesh, gl);
  }
}

void
object_update(Object* o)
{
  //TODO
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
