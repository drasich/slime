#include "player.h"
#include "object.h"
#include "property.h"
#include <Eet.h>
#include "scene.h"
#include "animation.h"

void
player_init(Component* c)
{
  Player* p = c->data;
  scene_reference_get(c->object->scene, &p->targetpointer);
  
  //*
  Eina_List* s = animation_list(c->object);

  Eina_List* l;
  char* name;
  EINA_LIST_FOREACH(s, l, name) {
    printf("anim name : %s \n", name);
  }
  //*/

  //animation_play(c->object, "idle", LOOP);
  //animation_play(c->object, "actiontest", LOOP);
  //animation_play(c->object, "realwalk", LOOP);
  animation_play(c->object, "ArmatureAction", LOOP);
  //animation_play(c->object, "twobone", LOOP);
  //animation_play(c->object, "moverotate", LOOP);
}

void
player_update(Component* c, double dt)
{
  Player* p = c->data;
  c->object->orientation_type = ORIENTATION_EULER;
  c->object->angles.x += p->rotation_speed*dt;
  if (p->target) {
    Vec3 pt = vec3_add(p->target->position, vec3(5,0,0));
    c->object->position = pt;
  }
}

Property* 
_player_properties(ComponentDesc* desc)
{
  Property *ps = property_set_new();

  PROPERTY_BASIC_ADD(ps, Player, name, EET_T_STRING);
  PROPERTY_BASIC_ADD(ps, Player, rotation_speed, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Player, my_test, EET_T_DOUBLE );


  int values[] = { enum_1, enum_2, enum_3, enum_4};
  //int values[] = {};
  const char* names[] = { "enum_1", "enum_2", "enum_3", "enum_4"};
  PROPERTY_ENUM_ADD(ps, Player, myenum, values, names);

  Property *obp = property_set_object_pointer("target");
  PROPERTY_SUB_NESTED_ADD(ps, Player, targetpointer, obp);

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

