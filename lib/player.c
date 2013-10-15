#include "player.h"
#include "object.h"
#include "property.h"
#include <Eet.h>

void
player_init(Component* c)
{
}

void
player_update(Component* c, double dt)
{
  Player* p = c->data;
  c->object->angles.x += p->rotation_speed*dt;
  if (p->target) {
    Vec3 pt = vec3_add(p->target->position, vec3(5,0,0));
    c->object->position = pt;
  }
}

PropertySet* 
_player_properties(ComponentDesc* desc)
{
  PropertySet *ps = create_property_set();

  ADD_PROP(ps, Player, name, EET_T_STRING);
  ADD_PROP(ps, Player, rotation_speed, EET_T_DOUBLE);
  ADD_PROP(ps, Player, target, PROPERTY_POINTER );
  ADD_PROP(ps, Player, my_test, EET_T_DOUBLE );

  return ps;
}

static void *
create_player()
{
  Player* p = calloc(1, sizeof *p);
  //Eina_Inarray* iarr = _player_properties();
  p->rotation_speed = 20;
  p->name = "chrisplay";

  //Component* c = create_component("player", player_funcs, p, iarr);
  return p;
}

ComponentDesc* component_player()
{
  ComponentDesc* cd = calloc(1, sizeof * cd);
  cd->name = "player";
  cd->create = create_player;
  cd->properties = _player_properties;
  cd->init = player_init;
  cd->update = player_update;
  return cd;
}

