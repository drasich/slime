#ifndef __scene__
#define __scene__
#include <Eina.h>
#include "object.h"

typedef struct _Scene Scene;

struct _Scene {
  //list of objects
  Eina_List* objects;
  struct View* v;
};

Scene* create_scene();
void scene_add_object(Scene* s, Object* o);
void scene_draw(Scene* s, int w, int h);
void scene_update(Scene* s);

#endif
