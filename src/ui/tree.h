#ifndef __ui_tree__
#define __ui_tree__
#include "object.h"
#include "context.h"

typedef struct _Tree Tree;

struct _Tree
{
  Evas_Object* root;
  Evas_Object* box;

  Context *context;
};

//void tree_update(Property* p, Object* o);
Tree* create_widget_tree(Evas_Object* win, Context* context);



#endif
