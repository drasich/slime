#include "prefab.h"
#include "log.h"
#include "resource.h"
#include "Eet.h"
#include "component.h"

static Property* _prefab_property = NULL;

void
prefab_del(Prefab* p)
{
  //object_del(p->prefab);
  components_del(p->components);

  //TODO unlink the other objects

  /*
  Eina_List* l;
  ObjectPointer* op;
  EINA_LIST_FOREACH(p->objects, l, op) {
    op->object->prefab = NULL;
  }

  eina_list_free(p->objects);
  */
}

Property*
property_set_prefab()
{
  //static Property* ps = NULL;
  Property* ps = _prefab_property;
  if (ps) return ps;

  ps = property_set_new();
  ps->name = "prefab";
  PROPERTY_SET_TYPE(ps, Prefab);

  /*
  Property* object_ps = property_set_object();
  EET_DATA_DESCRIPTOR_ADD_SUB(
        ps->descriptor, Prefab, "prefab", prefab,
        object_ps->descriptor);
        */

  EET_DATA_DESCRIPTOR_ADD_BASIC(
        ps->descriptor, Prefab, "name", name, EET_T_STRING);

  EET_DATA_DESCRIPTOR_ADD_LIST(
        ps->descriptor, Prefab, "components", components,
        component_descriptor);

  /*
  Property *obp = property_set_object_pointer("objects");

  EET_DATA_DESCRIPTOR_ADD_LIST(
        ps->descriptor, Prefab, "objects", objects,
        obp->descriptor);
        */

  return ps;
}

void
prefab_property_free()
{
  free(_prefab_property);
  _prefab_property = NULL;
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
  //Object* copy = object_copy(o);
  ComponentList* cl = components_copy(o->components);
  //object_post_read(copy, NULL);
  Prefab* prefab = calloc(1, sizeof *prefab);
  prefab->components = cl->list;
  prefab->name = o->name;
  free(cl);
  return prefab;
}

Object*
prefab_linked_object_new(Prefab* p)
{
  Object* o = object_new();
  o->prefab.name = p->name;
  o->prefab.prefab = p;
  return o;
}

Object*
prefab_copied_object_new(Prefab* p)
{
  Object* o = object_new();
  //o->prefab.name = p->name;
  //o->prefab.prefab = p;
  ComponentList* cl = components_copy(p->components);
  o->components = cl->list;
  object_post_read(o);
  free(cl);
  return o;
}


Object*
prefab_linked_object_new_by_name(const char* name)
{
  Prefab* p = resource_prefab_get(s_rm, name);
  return prefab_linked_object_new(p);
}

Object*
prefab_copied_object_new_by_name(const char* name)
{
  Prefab* p = resource_prefab_get(s_rm, name);
  return prefab_copied_object_new(p);
}

void 
prefab_post_read(Prefab* p)
{
  Eina_List* l;
  Eina_List* lnext;
  Component* c;

  EINA_LIST_FOREACH_SAFE(p->components, l, lnext, c) {
    if (!c->name) {
      EINA_LOG_DOM_WARN(log_object_dom, "It seems this component does not exist anymore: %s\n==> removing component", c->name);
      free(c);
      p->components = eina_list_remove_list(p->components, l);
    }
    else {
      EINA_LOG_DOM_DBG(log_object_dom, "post read, prefab : %s , component name : %s", p->name, c->name);
      c->funcs = component_manager_desc_get(s_component_manager, c->name);//TODO find from component manager;
      if (c->funcs) {
        EINA_LOG_DOM_DBG(log_object_dom, "component functions found, name : %s", c->name);
        c->properties = c->funcs->properties();
        //if (c->funcs->init)
        //c->funcs->init(c);
      }
      else {
        EINA_LOG_DOM_WARN(log_object_dom, "component functions NOT found, name: %s\n==> removing component", c->name);
        free(c);
        p->components = eina_list_remove_list(p->components, l);
      }
    }
  }

  /*
  Object* child;
  EINA_LIST_FOREACH(o->children, l, child) {
    object_post_read(child, s);
    child->parent = o;
  }
  */
}

