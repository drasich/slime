#include "scene.h"
#include "gl.h"

Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  eina_init();
  return s;
}

void
scene_add_object(Scene* s, Object* o)
{
   s->objects = eina_list_append(s->objects, o);

  Eina_List *l;
  void *list_data;
  EINA_LIST_FOREACH(s->objects, l, list_data)
   printf("%s\n", ((Object*)list_data)->name);
}

void
scene_destroy(Scene* s)
{
  Eina_List *l;
  void *list_data;
  //EINA_LIST_FOREACH(list, l, list_data)
   //printf("%s\n", (char*)list_data);

  eina_list_free(s->objects);

   eina_shutdown();
}

void
scene_draw(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_draw(o);
}

void
scene_update(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_update(o);
  
}
