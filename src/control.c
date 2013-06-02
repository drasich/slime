#include "control.h"
#include "context.h"
#include "object.h"
#include "view.h"

Control* 
create_control(View* v)
{
  Control* c = calloc(1, sizeof *c);
  c->state = IDLE;
  c->view = v;
  c->redo = NULL;
}

void
control_set_state(Control* c, int state)
{
  c->state = state;
}

Vec3
_objects_center(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->positions = eina_inarray_new (sizeof(Vec3), size);
  Eina_List *l;
  Object *o;
  Vec3 v = vec3_zero();
  EINA_LIST_FOREACH(objects, l, o) {
    vec3_add(v, o->Position);
    eina_inarray_push(c->positions, &o->Position);
  }

  vec3_mul(v, 1.0/(float)size);
}

void
control_move(Control* c)
{
  View* v = c->view;
  int x, y;
  //evas_pointer_output_xy_get(evas_object_evas_get(v->glview), &x, &y);
  evas_pointer_canvas_xy_get(evas_object_evas_get(v->glview), &x, &y);
  Vec2 mousepos = vec2(x,y);
  //printf("mouse pos : %f, %f \n", mousepos.X, mousepos.Y);
  Object* o = context_get_object(v->context);
  if (o != NULL && c->state != MOVE) {
    c->state = MOVE;
    //c->start = o->Position;
    c->start = _objects_center(c, context_get_objects(v->context));
    c->mouse_start = mousepos;
  }

}

void
control_center_camera(Control* c)
{
  View* v = c->view;
  Object* o = context_get_object(v->context);
  Camera* cam = v->camera;
  if (o != NULL && c->state == IDLE) {
    //TODO get the distance from the size of the object on the screen
    Vec3 v = vec3(0,0,30);
    v = quat_rotate_vec3(cam->object.Orientation, v);
    v = vec3_add(v, o->Position);

    //TODO write a camera_set_position function
    cam->object.Position = v;
    camera_recalculate_origin(cam);

  }

}


static void rotate_camera(View* v, float x, float y)
{
  Camera* cam = v->camera;
  Object* c = (Object*) cam;

  cam->yaw += 0.005f*x;
  cam->pitch += 0.005f*y;

  //TODO angles
  Quat qy = quat_angle_axis(cam->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(cam->pitch, vec3(1,0,0));
  Quat result = quat_mul(qy, qp);

  c->angles.X = cam->pitch/M_PI*180.0;
  c->angles.Y = cam->yaw/M_PI*180.0;

  Object* o = context_get_object(v->context);

  if (o != NULL) {
    if (!vec3_equal(o->Position, cam->center)) {
      cam->center = o->Position;
      camera_recalculate_origin(cam);
    }
  }

  camera_rotate_around(cam, result, cam->center);
}


void
control_mouse_move(Control* c, Evas_Event_Mouse_Move *e)
{
  c->mouse_current.X = e->cur.canvas.x;
  c->mouse_current.Y = e->cur.canvas.y;

  View* v = c->view;
  if (c->state == IDLE) {
    if (e->buttons & 1 == 1){
      float x = e->cur.canvas.x - e->prev.canvas.x;
      float y = e->cur.canvas.y - e->prev.canvas.y;

      const Evas_Modifier * mods = e->modifiers;
      if (evas_key_modifier_is_set(mods, "Shift")) {
        Vec3 t = {-x*0.05f, y*0.05f, 0};
        camera_pan(v->camera, t);
      } else {
        rotate_camera(v, x, y);
      }
    }
  } else if (c->state == MOVE) {
    Eina_List* objects = context_get_objects(v->context);
    Plane p = { c->start, quat_rotate_vec3(v->camera->object.Orientation, vec3(0,0,-1)) };

    Ray rstart = ray_from_screen(v->camera, c->mouse_start.X, c->mouse_start.Y, 1);

    float x = e->cur.canvas.x;
    float y = e->cur.canvas.y;
    Ray r = ray_from_screen(v->camera, x, y, 1);

    IntersectionRay ir =  intersection_ray_plane(r, p);
    IntersectionRay irstart =  intersection_ray_plane(rstart, p);
    
    if (ir.hit && irstart.hit) {
      Vec3 translation = vec3_sub(ir.position, irstart.position);
      Eina_List *l;
      Object *o;
      Vec3 v = vec3_zero();
      int i = 0;
      EINA_LIST_FOREACH(objects, l, o) {
        Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
        o->Position = vec3_add(*origin, translation);
        ++i;
      }
      //property_update(v->property, o);
    }
  }

}

static Operation* 
_op_move_object(Eina_List* objects, Vec3 start, Vec3 end)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_move_object_do;
  op->undo_cb = operation_move_object_undo;

  Op_Move_Object* omo = calloc(1, sizeof *omo);
  omo->objects = eina_list_clone(objects);
  omo->translation = vec3_sub(end, start);

  op->data = omo;

  return op;
}

static Operation* 
_op_add_object(Scene* s, Object* o)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_add_object_do;
  op->undo_cb = operation_add_object_undo;

  Op_Add_Object* od = calloc(1, sizeof *od);
  od->s = s;
  od->o = o;

  op->data = od;
  return op;
}

static Operation* 
//_op_remove_object(Scene* s, Object* o)
_op_remove_object(Scene* s, Eina_List* objects)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_remove_object_do;
  op->undo_cb = operation_remove_object_undo;

  Op_Remove_Object* od = calloc(1, sizeof *od);
  od->s = s;
  od->objects = eina_list_clone(objects);

  op->data = od;
  return op;
}



bool
control_mouse_down(Control* c, Evas_Event_Mouse_Down *e)
{
  if (c->state == MOVE) {
    c->state = IDLE;

    //TODO
    Object* o = context_get_object(c->view->context);
    Eina_List* objects = context_get_objects(c->view->context);

    Operation* op = _op_move_object(
          objects,
          c->start,
          o->Position);

    control_add_operation(c, op);
    return true;
  }

  return false;
}

void
control_add_object(Control* c, Scene* s, Object* o)
{
    Operation* op = _op_add_object(s,o);
    control_add_operation(c, op);
    op->do_cb(c, op->data);
}


void 
control_key_down(Control* c, Evas_Event_Key_Down *e)
{
  View* v = c->view;
  Scene* s = v->context->scene;
  Object* o = context_get_object(v->context);
  Eina_List* objects = context_get_objects(v->context);

  const Evas_Modifier * mods = e->modifiers;

  if (c->state == IDLE) {
    if (!strcmp(e->keyname, "Escape")) {
      elm_exit();
    } else if ( !strcmp(e->keyname, "g")) {
      //enter move mode
      control_move(c);
    } else if (!strcmp(e->keyname, "z") 
          && evas_key_modifier_is_set(mods, "Control")) {
      control_undo(c);
    } else if (!strcmp(e->keyname, "y")
          && evas_key_modifier_is_set(mods, "Control")) {
      control_redo(c);
    } else if (!strcmp(e->keyname, "f")) {
      if (o != NULL) {
        control_center_camera(c);
      }
    } else if (!strcmp(e->keyname, "x")) {
      if (o != NULL) {
        control_remove_object(c, s, objects);
      }
    } else if (!strcmp(e->keyname, "a")) {
      context_clean_objects(v->context);
    }

  } else if (c->state == MOVE) {
    if (!strcmp(e->keyname, "Escape")) {
      c->state = IDLE;

      // put back the objects to original position
      Eina_List *l;
      Object *o;
      int i = 0;
      EINA_LIST_FOREACH(objects, l, o) {
        Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
        o->Position = *origin;
        i++;
      }

    }
  }

}

void 
control_add_operation(Control* c, Operation* op)
{
  c->undo = eina_list_append(c->undo, op);
  control_clean_redo(c);
}


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

void
control_undo(Control* c)
{
  Eina_List* l = eina_list_last(c->undo);
  if (l) {
    Operation* op = l->data;
    op->undo_cb(c, op->data);
    c->redo = eina_list_append(c->redo, op);
    c->undo = eina_list_remove_list(c->undo, l);
  }
}

void
control_redo(Control* c)
{
  Eina_List* l = eina_list_last(c->redo);
  if (l) {
    Operation* op = l->data;
    op->do_cb(c, op->data);
    c->undo = eina_list_append(c->undo, op);
    c->redo = eina_list_remove_list(c->redo, l);
  }
}

void
control_clean_redo(Control* c)
{
  Operation *op;

  EINA_LIST_FREE(c->redo, op ) {
    //TODO might have to do a special callback for cleaning the operation
    free(op->data);
    free(op);
  }

}

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

void
//control_remove_object(Control* c, Scene* s, Object* o)
control_remove_object(Control* c, Scene* s, Eina_List* objects)
{
  Operation* op = _op_remove_object(s,objects);
  control_add_operation(c, op);
  op->do_cb(c, op->data);
}

