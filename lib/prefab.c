#include "prefab.h"
#include "log.h"
#include "resource.h"

void
prefab_del(Prefab* p)
{
  object_destroy(p->prefab);
  //TODO unlink the other objects

  Eina_List* l;
  ObjectPointer* op;
  EINA_LIST_FOREACH(p->objects, l, op) {
    op->object->prefab = NULL;
  }

  eina_list_free(p->objects);
}

Property*
property_set_prefab()
{
  static Property* ps = NULL;
  if (ps) return ps;

  ps = property_set_new();
  ps->name = "prefab";
  PROPERTY_SET_TYPE(ps, Prefab);

  Property* object_ps = property_set_object();
  EET_DATA_DESCRIPTOR_ADD_SUB(
        ps->descriptor, Prefab, "prefab", prefab,
        object_ps->descriptor);

  Property *obp = property_set_object_pointer("objects");

  EET_DATA_DESCRIPTOR_ADD_LIST(
        ps->descriptor, Prefab, "objects", objects,
        obp->descriptor);

  return ps;
}

static const char PREFAB_FILE_ENTRY[] = "prefab";

Eina_Bool
prefab_write(const Prefab* p, const char* filename)
{
  Eina_Bool ret;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_WRITE);
  if (!ef) {
    EINA_LOG_DOM_ERR(log_prefab_dom, "error reading file %s", filename);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, property_set_prefab()->descriptor, PREFAB_FILE_ENTRY, p, EINA_TRUE);
  eet_close(ef);

  return ret;
}

Prefab*
prefab_read(const char* filename)
{
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    EINA_LOG_DOM_ERR(log_prefab_dom, "error reading file '%s'.", filename);
    return NULL;
  }

  Prefab* p = eet_data_read(ef, property_set_prefab()->descriptor, PREFAB_FILE_ENTRY);
  eet_close(ef);
 
  return p;  
}

Prefab* 
prefab_new(const Object* o)
{
  Object* copy = object_copy(o);
  object_post_read(copy, NULL);
  Prefab* prefab = calloc(1, sizeof *prefab);
  prefab->prefab = copy;
  return prefab;
}

Object*
prefab_object_new(Prefab* p)
{
  Object* copy = object_copy(p->prefab);
  object_post_read(copy, NULL);
  copy->prefab = p;
  p->objects = eina_list_append(p->objects, copy);
  return copy;
}

Object*
prefab_object_new_by_name(const char* name)
{
  Prefab* p = resource_prefab_get(s_rm, name);
  return prefab_object_new(p);
}
