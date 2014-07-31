#ifndef __component_player__
#define __component_player__
#include "stdbool.h"
#include "component.h"
#include "object_pointer.h"

typedef enum _MyEnum MyEnum;

enum _MyEnum
{
  enum_1,
  enum_2,
  enum_3,
  enum_4,
};


typedef struct _Player Player;


struct _Player
{
  const char* name;
  int translation_state;
  double rotation_speed;
  double my_test;
  bool rotate;
  Object* target;
  ObjectPointer targetpointer;
  MyEnum myenum;
};

void player_init(Component* c);
void player_update(Component* c, double dt);

//Component* create_player();
ComponentDesc* component_player();
#endif
