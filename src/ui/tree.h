#ifndef __ui_tree__
#define __ui_tree__
//#include "object.h"
#include "context.h"
#include "view.h"

typedef struct _Tree Tree;

struct _Tree
{
  Evas_Object* root;
  Evas_Object* box;
  Evas_Object* gl;

  Context* context;
  View* view;
  Control* control;
};

//Tree* create_widget_tree(Evas_Object* win, Context* context);
Tree* create_widget_tree(Evas_Object* win, View* view);
void tree_add_object(Tree* t,  struct _Object* o);
void tree_remove_object(Tree* t,  struct _Object* o);

void tree_select_object(Tree* t, struct _Object* o);
void tree_select_objects(Tree* t, Eina_List* objects);
void tree_update_object(Tree* t, struct _Object* o);

void tree_scene_set(Tree* t, struct _Scene* s);

void tree_unselect_all(Tree* t);

#endif
