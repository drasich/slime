#include "core.h"
#include "object.h"
#include "property.h"
#include <Eet.h>
#include "scene.h"
#include "component/meshcomponent.h"

typedef enum _Direction Direction;
enum _Direction {
  DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN, DIR_END
};

static Direction
_random_direction_get()
{
  Direction dir = rand() % DIR_END;
  return dir;
}

static void
_core_begin(Object* owner, Core* c)
{
  printf("core begin %s \n", owner->name );
  int i;
  for (i = 0; i < 10; ++i) {
    char s[2];
    eina_convert_itoa(i, &s[0]);
    Object* o = prefab_copied_object_new_by_name("new_empty");//core");
    o->name = eina_stringshare_add(s);
    object_child_add(owner, o);
    //o->position.x = (i +1) * c->margin;
    Direction dir = _random_direction_get();
    if (dir == DIR_LEFT)
    o->position.x = -(i +1) * c->margin;
    else if (dir == DIR_RIGHT)
    o->position.x = (i +1) * c->margin;
    else if (dir == DIR_DOWN)
    o->position.y = -(i +1) * c->margin;
    else if (dir == DIR_UP)
    o->position.y = (i +1) * c->margin;

    Color col = rand() % COLOR_END;
  }

  c->state = 1;
  printf("core begin end %d \n", c->state);
}

void
core_init(Component* c)
{
  Core* core = c->data;
  core->state = 0;
  printf("core init\n");
}

void
core_update(Component* c, double dt)
{
  //return;
  Core* core = c->data;
  //printf("core update %d \n", core->state );

  if (core->state == 0) {
    _core_begin(c->object, core);
    core->state = 1;
  }
}

Property* 
_core_properties(ComponentDesc* desc)
{
  Property *ps = property_set_new();

  PROPERTY_BASIC_ADD(ps, Core, name, EET_T_STRING);
  PROPERTY_BASIC_ADD(ps, Core, rotation_speed, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Core, margin, EET_T_FLOAT );
  PROPERTY_BASIC_ADD(ps, Core, zzz, EET_T_FLOAT );

  //Property *obp = property_set_object_pointer("target");
  //PROPERTY_SUB_NESTED_ADD(ps, Player, targetpointer, obp);

  return ps;
}

static void *
create_core()
{
  printf("core create\n");
  Core* p = calloc(1, sizeof *p);
  p->rotation_speed = 20;
  p->name = "chrisplay";
  return p;
}

ComponentDesc* component_core()
{
  ComponentDesc* cd = calloc(1, sizeof * cd);
  cd->name = "core";
  cd->create = create_core;
  cd->properties = _core_properties;
  cd->init = core_init;
  cd->update = core_update;
  return cd;
}

