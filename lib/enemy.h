#ifndef __component_enemy__
#define __component_enemy__
#include "stdbool.h"
#include "component.h"
#include "object.h"

typedef struct _Enemy Enemy;

struct _Enemy
{
  const char* name;
  double speed;
  Object* object;
  bool movepos;
};

void enemy_init(Component* c);
void enemy_update(Component* c, double dt);

Component* create_enemy();

#endif
