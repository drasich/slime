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
  Elm_Object_Item* group;
  Elm_Object_Item* scene_group_playing;
  ResourceType resource_type;
};

ResourceView* resource_view_new(Evas_Object* win, View* v, ResourceType type);
void resource_view_scene_add(ResourceView* rv, const Scene* s);
void resource_view_playing_scene_add(ResourceView* rv, const Scene* s);
void resource_view_scene_del(ResourceView* rv, const Scene* s);
void resource_view_scene_select(ResourceView* rv, const Scene* s);

void resource_view_update(ResourceView* rv);
void resource_view_clean(ResourceView* rv);

void resource_view_prefab_add(ResourceView* rv, const Prefab* o);

Elm_Object_Item* resource_view_group_add(ResourceView* rv, const char* name);

typedef struct _ResourceGroup ResourceGroup;
struct _ResourceGroup
{
  const char* name;
  Eina_Hash* resources;
  Elm_Object_Item* item;
};

#endif
