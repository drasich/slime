#include "game.h"
#include "enemy.h"
#include "player.h"

Eina_List*
create_components()
{
  Eina_List* l;
  l = eina_list_append(l,create_enemy());
  l = eina_list_append(l,create_player());

  return l;
}

