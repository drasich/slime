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
  Eina_Inarray *arr;
  Eina_Hash *properties;
  //PropertyView *pw;
  // add callbacks here if we want to call something when data has changed.
  property_changed callback;
  Control* control;

  void* data;
  const char* value_saved;
  const char* name;
};

ComponentProperties* create_my_prop(const char* name, Eina_Inarray *a, Evas_Object* win, Control* control);

void _property_update_data(ComponentProperties* mp, void* data);

#endif