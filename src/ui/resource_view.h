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
  Elm_Object_Item* scene_group;
};

ResourceView* resource_view_new(Evas_Object* win, View* v);
void resource_view_scene_add(ResourceView* rv, const Scene* s);

Elm_Object_Item* resource_view_group_add(ResourceView* rv, const char* name);

#endif
