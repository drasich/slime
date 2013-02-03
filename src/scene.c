#include "scene.h"
#include "gl.h"

Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  eina_init();
  s->camera = create_object();
  s->camera->name = "camera";
  Vec3 v = {10,10,10};
  //Vec3 axis = {0,1,0};
  //Quat q = quat_angle_axis(3.14f/4.0f, axis);
  s->camera->Position = v;
  Vec3 at = {0,0,0};
  Vec3 up = {0,1,0};
  s->camera->Orientation = quat_lookat(v, at, up);

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

  o->scene = s;
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
scene_draw(Scene* s, int w, int h)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_draw(o, w, h, s->camera);
}

void
scene_update(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_update(o);
  
}
