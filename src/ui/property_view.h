#ifndef __ui_property_view__
#define __ui_property_view__
#include "object.h"
#include "context.h"
#include "control.h"
#include "component_view.h"

typedef struct _PropertyView PropertyView;

struct _PropertyView
{
  Evas_Object* root;
  Evas_Object* box;

  Context *context;
  Control *control;
  Evas_Object* win;
  PropertySet *arr;
  PropertySet *array_multiple_objects;
  ComponentProperties* current;
  Evas_Object* scroller;

  Eina_Hash *component_widgets_backup;
  Eina_List *component_widgets;
};

void property_update(PropertyView* p, Eina_List* objects);
void property_update_data(PropertyView* p, Eina_List* objects);
PropertyView* create_property(Evas_Object* win, Context* context, Control* control);
void property_set(PropertyView* pw, ComponentProperties* mp);

void property_update_components(PropertyView* pw, Object* o);
void property_update_components_data(PropertyView* pw, Object* o);



#endif
