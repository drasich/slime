#ifndef __prefab__
#define __prefab__
#include "Eina.h"
#include "object.h"

typedef struct _Prefab Prefab;
struct _Prefab
{
  Object* prefab;
  Eina_List* objects;
};

Prefab* prefab_new(const Object* o);
void prefab_del(Prefab* p);
Property* property_set_prefab();

Eina_Bool prefab_write(const Prefab* p, const char* filename);
Prefab* prefab_read(const char* filename);

Object* prefab_object_new(Prefab* p);
Object* prefab_object_new_by_name(const char* name);

#endif
