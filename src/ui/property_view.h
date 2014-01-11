#ifndef __ui_property_view__
#define __ui_property_view__
#include "object.h"
#include "context.h"
#include "control.h"
#include "component_view.h"
#include "prefab.h"

typedef struct _PropertyView PropertyView;

struct _PropertyView
{
  Evas_Object* root;
  Evas_Object* box;

  Context *context;
  Control *control;
  Evas_Object* win;
  //PropertySet *arr;
  //PropertySet *array_multiple_objects;
  Evas_Object* scroller;

  Eina_List *component_widgets;
  Object* object;
};

void property_update(PropertyView* p, Eina_List* objects);
PropertyView* create_property(Evas_Object* win, Context* context, Control* control);
void property_set(PropertyView* pw, ComponentProperties* mp);

void property_object_show(PropertyView* pw, Object* o);
void property_update_components_data(PropertyView* pw, Component* component);
void property_update_component(PropertyView* pw, const char* name);

void property_reload_component(PropertyView* pw, Component* c);

void property_scene_show(PropertyView* pw, Scene* s);
void property_prefab_show(PropertyView* pw, Prefab* p);

typedef struct _PropertyHolder PropertyHolder;
struct _PropertyHolder
{
  Eina_List* objects;
  Eina_List* items;
  const Property* property;
};

void property_holder_init();

void property_holder_object_add(const void* data, const Property* p, Evas_Object* o);
void property_holder_genlist_item_add(const void* data, Elm_Object_Item* i);
void property_holder_object_del(void* data, Evas_Object* o);
void property_holder_del(void* data);
void property_holder_update(void* data);

#endif
