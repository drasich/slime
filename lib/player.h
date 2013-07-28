#ifndef __component_player__
#define __component_player__
#include "stdbool.h"
#include "component.h"

typedef struct _Player Player;

struct _Player
{
  const char* name;
  int translation_state;
  double rotation_speed;
  bool rotate;
};

void player_init(Component* c);
void player_update(Component* c, double dt);

//Component* create_player();
ComponentDesc player_desc;

#endif
