#ifndef __context__
#define __context__

typedef struct _Context Context;

struct _Context{
  struct _Object* object;
  struct _Scene* scene;
};

void context_set_object(Context* c, struct _Object* o);

#endif
