#ifndef __ui_tree__
#define __ui_tree__
//#include "object.h"
#include "context.h"

typedef struct _Tree Tree;

struct _Tree
{
  Evas_Object* root;
  Evas_Object* box;
  Evas_Object* gl;

  Context *context;
};

//void tree_update(Property* p, Object* o);
Tree* create_widget_tree(Evas_Object* win, Context* context);
void tree_add_object(Tree* t,  struct _Object* o);
void tree_remove_object(Tree* t,  struct _Object* o);

void tree_update(Tree* t, struct _Object* o);


#endif
