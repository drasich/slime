#ifndef __ui_component_view__
#define __ui_component_view__
#include "object.h"
#include "context.h"
#include "control.h"

typedef struct _ComponentProperties ComponentProperties;

typedef void (*property_changed)(Control* control, void* data, Property* p);
struct _ComponentProperties
{
  Evas_Object* win;
  Evas_Object* box;
  const Property *property;
  Eina_Hash *properties;
  struct _PropertyView *pw;
  // add callbacks here if we want to call something when data has changed.
  property_changed callback;
  Control* control;

  const char* value_saved;
  const char* name;
  Evas_Object* root;
  Component* component;

  Eina_Value saved;
  Quat quat_saved;
  Eina_List* entries;

};

ComponentProperties* create_my_prop(Component* c, Evas_Object* win, Control* control, bool can_remove);
ComponentProperties* create_component_properties(Component* c, struct _PropertyView* pw);

void component_property_update_data(ComponentProperties* mp, void* data);

#endif
