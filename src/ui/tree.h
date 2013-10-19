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

  Eina_Hash* objects;
};

Tree* tree_widget_new(Evas_Object* win, View* view);
void tree_object_add(Tree* t,  struct _Object* o);
void tree_object_remove(Tree* t,  struct _Object* o);

void tree_object_select(Tree* t, struct _Object* o);
void tree_objects_select(Tree* t, Eina_List* objects);
void tree_object_update(Tree* t, struct _Object* o);

void tree_scene_set(Tree* t, struct _Scene* s);

void tree_unselect_all(Tree* t);

#endif
