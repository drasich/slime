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
  Evas_Object* win;
  Eina_Inarray *arr;
};

Evas_Object* property_add_entry(
      Evas_Object* win, 
      Evas_Object* bx, 
      char* name, 
      char* value);

void property_update(Property* p, Eina_List* objects);
Property* create_property(Evas_Object* win, Context* context);

typedef struct
{
  const char* name;
  int type;
  int offset;
} Prop;

#define ADD_PROP(array, struct_type, member, member_type) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Prop p = { # member, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett))};                \
    eina_inarray_push(array, &p); \
  } while(0)

Eina_Inarray* create_property_set();


#endif
