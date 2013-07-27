#include "enemy.h"
#include "object.h"
#include "property.h"
#include <Eet.h>

void
enemy_init(Component* c)
{
}

void
enemy_update(Component* c, double dt)
{
  Enemy* e = c->data;
  c->object->Position.X += e->speed*dt;
  if (e->movepos && c->object->Position.X > 5)
  c->object->Position.X -= e->speed*dt;
  else if (!e->movepos && c->object->Position.X <- 5)
  c->object->Position.X += e->speed*dt;
}

static Eina_Inarray* 
_enemy_properties()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, Enemy, name, EET_T_STRING);
  ADD_PROP(iarr, Enemy, speed, EET_T_DOUBLE);

  return iarr;
}

ComponentFuncs enemy_funcs = { enemy_init, enemy_update };

Component*
create_enemy()
{
  Enemy* e = calloc(1, sizeof *e);
  Eina_Inarray* iarr = _enemy_properties();
  e->speed = 20;

  Component* c = create_component("enemy", enemy_funcs, e, iarr);
  return c;
}

