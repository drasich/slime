#ifndef __prefab__
#define __prefab__
#include "Eina.h"
#include "object.h"

typedef struct _Prefab Prefab;
struct _Prefab
{
  const char* name;
  Eina_List* components;
};

Prefab* prefab_new(const Object* o);
void prefab_del(Prefab* p);
Property* property_set_prefab();
void prefab_property_free();

Eina_Bool prefab_write(const Prefab* p, const char* filename);
Prefab* prefab_read(const char* filename);

Object* prefab_linked_object_new(Prefab* p);
Object* prefab_copied_object_new(Prefab* p);
Object* prefab_linked_object_new_by_name(const char* name);
Object* prefab_copied_object_new_by_name(const char* name);
void prefab_post_read(Prefab* p);

#endif
