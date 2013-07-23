#ifndef __ui_property_view__
#define __ui_property_view__
#include "object.h"
#include "context.h"
#include "control.h"

typedef struct _PropertyView PropertyView;
typedef struct _MyProp MyProp;

struct _PropertyView
{
  Evas_Object* root;
  Evas_Object* box;

  Context *context;
  Control *control;
  //Eina_Hash *properties;
  Evas_Object* win;
  Eina_Inarray *arr;
  Eina_Inarray *array_multiple_objects;
  MyProp* current;
  Evas_Object* scroller;

  MyProp* oneobj;
  MyProp* manyobj;
  Eina_Hash *component_widgets;
};

typedef void (*property_changed)(Control* control, void* data, Property* p);
struct _MyProp
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
};

Evas_Object* property_add_entry(
      Evas_Object* win, 
      Evas_Object* bx, 
      char* name, 
      char* value);

void property_update(PropertyView* p, Eina_List* objects);
PropertyView* create_property(Evas_Object* win, Context* context, Control* control);
void property_set(PropertyView* pw, MyProp* mp);
void _property_update_data(MyProp* mp, void* data);



#endif
