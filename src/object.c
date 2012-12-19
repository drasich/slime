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
object_draw(Object* o)
{
  //TODO
}

void
object_update(Object* o)
{
  //TODO
}

void
object_add_component(Component* c)
{
  //TODO
}
