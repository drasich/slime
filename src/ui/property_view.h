#ifndef __ui_property_view__
#define __ui_property_view__
#include "object.h"
#include "context.h"
#include "control.h"

typedef struct _PropertyView PropertyView;

struct _PropertyView
{
  Evas_Object* root;
  Evas_Object* box;

  Context *context;
  Control *control;
  Eina_Hash *properties;
  Evas_Object* win;
  Eina_Inarray *arr;
  Eina_Inarray *array_objects;
};

Evas_Object* property_add_entry(
      Evas_Object* win, 
      Evas_Object* bx, 
      char* name, 
      char* value);

void property_update(PropertyView* p, Eina_List* objects);
PropertyView* create_property(Evas_Object* win, Context* context, Control* control);

#endif
