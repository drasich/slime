#include "context.h"

void
context_set_object(Context* c, struct _Object* o)
{
  context_add_object(c, o);
}

void
context_clean_objects(Context* c)
{
  c->objects = eina_list_free(c->objects);
  c->object = NULL;
}

void
context_add_object(Context* c, struct _Object* o)
{
  if (!eina_list_data_find_list(c->objects, o)) {
    c->objects = eina_list_append(c->objects, o);
  }
  c->object = o;
}

void
context_remove_object(Context* c, struct _Object* o)
{
  c->objects = eina_list_remove(c->objects, o);
}

struct _Object* 
context_get_object(Context* c)
{
  return c->object;
  //return eina_list_data_get(eina_list_last(c->objects));
}

Eina_List*
context_get_objects(Context* c)
{
  return c->objects;
}
