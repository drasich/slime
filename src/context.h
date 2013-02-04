#ifndef __context__
#define __context__

typedef struct _Context Context;

struct _Context{
  struct _Object* object;
  struct _Scene* scene;
};

#endif
