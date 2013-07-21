#ifndef __component_player__
#define __component_player__
#include "stdbool.h"

typedef struct _Player player;

struct _Player
{
  int translation_state;
  bool rotate;
};

void player_init();
void player_update();

#endif
