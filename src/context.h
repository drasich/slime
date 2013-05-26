#ifndef __context__
#define __context__
#include <Eina.h>

typedef struct _Context Context;

struct _Context{
  Eina_List* objects;
  struct _Object* object;
  struct _Scene* scene;
};

struct _Object;

void context_set_object(Context* c, struct _Object* o);

void context_clean_objects(Context*c);
void context_add_object(Context* c, struct _Object* o);
void context_remove_object(Context* c, struct _Object* o);

struct _Object* context_get_object(Context* c);
Eina_List* context_get_objects(Context* c);

#endif
