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

/////////////////////////////////

static void
_change_data(Object* o, Property* p, void *data)
{
  switch(p->type) {
    case EET_T_DOUBLE:
       {
        memcpy((void*)o + p->offset, data, sizeof(double));
       }
      break;
    case EET_T_STRING:
       {
        const char** str = (void*)o + p->offset;
        eina_stringshare_del(*str);
        const char* s = data;
        *str = eina_stringshare_add(s);
        //eina_stringshare_dump();
       }
      break;
    default:
      fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
      break;
   }

  //TODO update the entry

}

void 
operation_change_property_do(Control *c, void* data)
{
  Op_Change_Property* opd = data;
  Property* p = opd->p;
  Object* o = opd->o;
  _change_data(o, p, opd->value_new);
}

void
operation_change_property_undo(Control *c, void* data)
{
  Op_Change_Property* opd = data;
  Property* p = opd->p;
  Object* o = opd->o;
  _change_data(o, p, opd->value_old);
}


