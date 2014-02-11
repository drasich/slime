#ifndef __component_core__
#define __component_core__
#include "stdbool.h"
#include "component.h"
#include "object_pointer.h"

//typedef enum _Color Color;
typedef enum {
  COLOR_BLUE, COLOR_RED, COLOR_YELLOW, COLLOR_VIOLET, COLOR_END
} Color;


typedef struct _Core Core;
struct _Core
{
  const char* name;
  int state;
  double rotation_speed;
  ObjectPointer targetpointer;
  float margin;
  float zzz;
  Color color;

};

void core_init(Component* c);
void core_update(Component* c, double dt);

ComponentDesc* component_core();
#endif
