#ifndef __ui_property__
#define __ui_property__
#include "object.h"
#include "context.h"

typedef struct _Property Property;

struct _Property
{
  Evas_Object* root;
  Evas_Object* box;

  Context *context;
  Eina_Hash *properties;
};

Evas_Object* property_create(Evas_Object* win);

Evas_Object* property_add(
      Evas_Object* win, 
      Evas_Object* bx, 
      char* name, 
      char* value);

void property_update(Property* p, Eina_List* objects);
Property* create_property(Evas_Object* win, Context* context);



#endif
