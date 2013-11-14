#include "operation.h"
#include "view.h"
#include "resource.h"
#include "ui/tree.h" //TODO remove

void 
operation_move_object_do(Control* c, void* data)
{
  Op_Move_Object* od = (Op_Move_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    o->position = vec3_add(o->position, od->translation);
  }

}

void 
operation_move_object_undo(Control* c, void* data)
{
  Op_Move_Object* od = (Op_Move_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    o->position = vec3_sub(o->position, od->translation);
  }
}

////////////////


void 
operation_add_object_do(Control* c, void* data)
{
  Op_Add_Object* od = (Op_Add_Object*) data;
  scene_add_object(od->s, od->o);
  tree_object_add(c->view->tree,  od->o);
}

void 
operation_add_object_undo(Control*c, void* data)
{
  Op_Add_Object* od = (Op_Add_Object*) data;
  scene_remove_object(od->s, od->o);
  tree_object_remove(c->view->tree,  od->o);

  Object* o = context_object_get(c->view->context);

  if (od->o == o &&  od->s == c->view->context->scene){
    //c->view->context->object = NULL;
    context_object_remove(c->view->context, o);
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
    tree_object_remove(c->view->tree,  o);
    if (od->s == context->scene){
      if (eina_list_data_find(context->objects, o)) {
        context_object_remove(context, o);
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
    tree_object_add(c->view->tree,  o);
  }

}

/////////////////////////////////

static void
_change_data(void* c, Property* p, const void *data)
{
  switch(p->type) {
    case EET_T_DOUBLE:
       {
        memcpy((void*)c + p->offset, data, sizeof(double));
       }
      break;
    case EET_T_STRING:
       {
        const char** str = (void*)c + p->offset;
        //eina_stringshare_del(*str);
        const char* s = data;
        //*str = eina_stringshare_add(s);
        *str = s;
        //eina_stringshare_dump();
       }
      break;
    case PROPERTY_POINTER:
       {
        //const char** str = (void*)c + p->offset;
        //const char* s = data;
        //*str = s;
        //printf("new pointer : %s\n", s);


        const void** ptr = (void*)c + p->offset;
        *ptr = data;
       }
      break;
    case PROPERTY_QUAT:
      memcpy(c+p->offset, data, sizeof(Quat));
      break;
    case PROPERTY_RESOURCE:
      memcpy(c+p->offset, data, sizeof(ResourceHandle));
      break;
    case PROPERTY_UNIFORM:
       {
        //int offset = property_offset_get(p);
        //UniformValue* uv = c + offset;
        UniformValue* uv = c;
        if (uv->type == UNIFORM_FLOAT) {
          const float* f = data;
          uv->value.f = *f;
        }
        else if (uv->type == UNIFORM_VEC3) {
          const Vec3* v = data;
          uv->value.vec3 = *v;
        }
        else {
          fprintf (stderr, "_change_data: uniform not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
        }
       }
      break;
    default:
      fprintf (stderr, "_change_data: type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
      break;
   }
}

void 
operation_change_property_do(Control *c, void* data)
{
  Op_Change_Property* opd = data;
  _change_data(opd->data, opd->p, opd->value_new);
  control_property_update(c, opd->component);
}

void
operation_change_property_undo(Control *c, void* data)
{
  Op_Change_Property* opd = data;
  _change_data(opd->data, opd->p, opd->value_old);
  control_property_update(c, opd->component);
}


////////////////////////////////////////////////////////
void 
operation_object_add_component_do(Control* c, void* data)
{
  Op_Object_Add_Component* od = (Op_Object_Add_Component*) data;
  object_add_component(od->o, od->c);
  control_on_object_components_changed(c, od->o);
}

void 
operation_object_add_component_undo(Control*c, void* data)
{
  Op_Object_Add_Component* od = (Op_Object_Add_Component*) data;
  object_remove_component(od->o, od->c);
  //TODO delete the component in the cleaning of operations or when we quit the application
  control_on_object_components_changed(c, od->o);
}



////////////////////////////////////////////////////////////
void 
operation_scale_object_do(Control* c, void* data)
{
  Op_Scale_Object* od = (Op_Scale_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    o->scale = vec3_vec3_mul(o->scale, od->scale);
  }

}

void 
operation_scale_object_undo(Control* c, void* data)
{
  Op_Scale_Object* od = (Op_Scale_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    o->scale = vec3_vec3_mul(
          o->scale,
          vec3(
            1.0f/od->scale.x,
            1.0f/od->scale.y,
            1.0f/od->scale.z
            )
          );
  }
}

////////////////

void 
operation_rotate_object_do(Control* c, void* data)
{
  Op_Rotate_Object* od = (Op_Rotate_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    if (od->local)
    o->orientation = quat_mul(o->orientation, od->quat);
    else
    o->orientation = quat_mul(od->quat, o->orientation);
  }

}

void 
operation_rotate_object_undo(Control* c, void* data)
{
  Op_Rotate_Object* od = (Op_Rotate_Object*) data;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(od->objects, l, o) {
    Quat invrot = quat_inverse(od->quat);
    Quat q = quat_inverse(od->quat);
    if (od->local)
    o->orientation = quat_mul(o->orientation, invrot);
    else
    o->orientation = quat_mul(invrot, o->orientation);
  }
}

