#include "game.h"
#include "enemy.h"
#include "player.h"
#include "core.h"

Eina_List*
create_components()
{
  Eina_List* l = NULL;
  //l = eina_list_append(l, &player_desc);
  l = eina_list_append(l, component_player());
  //l = eina_list_append(l,create_enemy());
  //l = eina_list_append(l,create_player());
  l = eina_list_append(l, component_core());

  return l;
}

