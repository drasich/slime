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

static Eina_Inarray* 
_player_properties()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, Player, name, EET_T_STRING);
  ADD_PROP(iarr, Player, rotation_speed, EET_T_DOUBLE);

  return iarr;
}

ComponentFuncs player_funcs = { player_init, player_update };

Component*
create_player()
{
  Player* p = calloc(1, sizeof *p);
  Eina_Inarray* iarr = _player_properties();
  p->rotation_speed = 20;

  Component* c = create_component("player", player_funcs, p, iarr);
  return c;
}
