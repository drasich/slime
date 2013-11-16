#ifndef __ui_resource__
#define __ui_resource__
#include "view.h"
#include "scene.h"

typedef struct _ResourceView ResourceView;

struct _ResourceView
{
  Evas_Object* root;
  Evas_Object* gl;

  View* view;
  //Control* control;
  Eina_Hash* scenes;
};

ResourceView* resource_view_new(Evas_Object* win, View* v);
void resource_view_scene_add(ResourceView* rv, const Scene* s);

#endif
