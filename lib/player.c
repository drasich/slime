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
  c->object->angles.X += p->rotation_speed*dt;
}

PropertySet* 
_player_properties(ComponentDesc* desc)
{
  PropertySet *ps = create_property_set();
  Eina_Inarray * iarr = ps->array;

  ADD_PROP(iarr, Player, name, EET_T_STRING);
  ADD_PROP(iarr, Player, rotation_speed, EET_T_DOUBLE);

  return ps;
}

static void *
create_player()
{
  Player* p = calloc(1, sizeof *p);
  //Eina_Inarray* iarr = _player_properties();
  p->rotation_speed = 20;

  //Component* c = create_component("player", player_funcs, p, iarr);
  return p;
}

ComponentDesc player_desc = { 
  "player",
  create_player,
  _player_properties,
  player_init, 
  player_update,
};

