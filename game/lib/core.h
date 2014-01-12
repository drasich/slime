#ifndef __component_core__
#define __component_core__
#include "stdbool.h"
#include "component.h"
#include "object_pointer.h"

typedef struct _Core Core;

struct _Core
{
  const char* name;
  int state;
  double rotation_speed;
  ObjectPointer targetpointer;
  float margin;
};

void core_init(Component* c);
void core_update(Component* c, double dt);

ComponentDesc* component_core();
#endif
