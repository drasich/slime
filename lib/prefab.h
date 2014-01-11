#ifndef __prefab__
#define __prefab__
#include "Eina.h"
#include "object.h"

typedef struct _PrefabManager PrefabManager;
struct _PrefabManager
{
  Eina_Hash* prefabs;
};

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

#endif
