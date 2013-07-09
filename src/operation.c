#include "operation.h"
#include "view.h"

void 
operation_move_object_do(Control* c, void* data)
{
  Op_Move_Object* od = (Op_Move_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    o->Position = vec3_add(o->Position, od->translation);
  }

}

void 
operation_move_object_undo(Control* c, void* data)
{
  Op_Move_Object* od = (Op_Move_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    o->Position = vec3_sub(o->Position, od->translation);
  }
}

////////////////


void 
operation_add_object_do(Control* c, void* data)
{
  Op_Add_Object* od = (Op_Add_Object*) data;
  scene_add_object(od->s, od->o);
  tree_add_object(c->view->tree,  od->o);
}

void 
operation_add_object_undo(Control*c, void* data)
{
  Op_Add_Object* od = (Op_Add_Object*) data;
  scene_remove_object(od->s, od->o);
  tree_remove_object(c->view->tree,  od->o);

  Object* o = context_get_object(c->view->context);

  if (od->o == o &&  od->s == c->view->context->scene){
    //c->view->context->object = NULL;
    context_remove_object(c->view->context, o);
  }
  //TODO context if object was the object in the context remove it
}

///////////////////////////


void 
operation_remove_object_do(Control *c, void* data)
{
  Op_Remove_Object* od = (Op_Remove_Object*) data;
  Context* context = c->view->context;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    scene_remove_object(od->s, o);
    tree_remove_object(c->view->tree,  o);
    if (od->s == context->scene){
      if (eina_list_data_find(context->objects, o)) {
        context_remove_object(context, o);
      }
    }
  }
}

void
operation_remove_object_undo(Control *c, void* data)
{
  Op_Remove_Object* od = (Op_Remove_Object*) data;
  Context* context = c->view->context;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    scene_add_object(od->s, o);
    tree_add_object(c->view->tree,  o);
  }

}


