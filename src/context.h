#ifndef __context__
#define __context__
#include <Eina.h>
#include "vec.h"


typedef struct _Multiple_Objects_Selection Multiple_Objects_Selection;
struct _Multiple_Objects_Selection{
  Vec3 center;
  Vec3 rotation;
};

typedef struct _Context Context;
typedef void (*context_cb)(Context* c, void* listener, const char* msg);


struct _Context{
  Eina_List* objects;
  struct _Object* object;
  struct _Scene* scene;
  Multiple_Objects_Selection mos;
  Eina_List* cb;
  Eina_List* listener;
};

struct _Object;

void context_object_set(Context* c, struct _Object* o);

void context_objects_clean(Context*c);
void context_object_add(Context* c, struct _Object* o);
void context_object_remove(Context* c, struct _Object* o);
void context_objects_set(Context* c, Eina_List* objects);

struct _Object* context_object_get(Context* c);
Eina_List* context_objects_get(Context* c);


void context_add_callback(Context* c, context_cb cb, void* listener);

bool context_object_contains(Context* c, struct _Object* o);


#endif
