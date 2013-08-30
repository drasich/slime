#include "context.h"

static void
_context_send_message(Context* c, const char* msg)
{
  Eina_List *l;
  context_cb cb;
  int i = 0;


  EINA_LIST_FOREACH(c->cb, l, cb) {
    void* l = eina_list_nth(c->listener, i);
    cb(c, l, msg);
    ++i;
  }
}

void
context_object_set(Context* c, struct _Object* o)
{
  context_add_object(c, o);

}

void
context_clean_objects(Context* c)
{
  c->objects = eina_list_free(c->objects);
  c->object = NULL;
  _context_send_message(c, "clean_objects");
}

void
context_add_object(Context* c, struct _Object* o)
{
  if (!eina_list_data_find_list(c->objects, o)) {
    c->objects = eina_list_append(c->objects, o);

    _context_send_message(c, "add_object");
  }

  if (c->object != o) {
    c->object = o;
    //_context_send_message(c, "mainobject_changed");
  }
}

void
context_remove_object(Context* c, struct _Object* o)
{
  c->objects = eina_list_remove(c->objects, o);
  _context_send_message(c, "remove_object");
}

struct _Object* 
context_object_get(Context* c)
{
  return c->object;
  //return eina_list_data_get(eina_list_last(c->objects));
}

Eina_List*
context_objects_get(Context* c)
{
  return c->objects;
}

void
context_add_callback(Context* c, context_cb cb, void* listener)
{
  c->cb = eina_list_append(c->cb, cb);
  c->listener = eina_list_append(c->listener, listener);
}

