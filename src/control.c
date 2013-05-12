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

void
control_move(Control* c)
{
  View* v = c->view;
  Object* o = v->context->object;
  if (o != NULL && c->state != MOVE) {
    c->state = MOVE;
    c->start = o->Position;
  }

}

void
control_center_camera(Control* c)
{
  View* v = c->view;
  Object* o = v->context->object;
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

  Object* o = v->context->object;

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
    Object* o = v->context->object;
    if (o != NULL) {
      float x = e->cur.canvas.x;
      float y = e->cur.canvas.y;
      Ray r = ray_from_screen(v->camera, x, y, 1);
      Plane p = { c->start, quat_rotate_vec3(v->camera->object.Orientation, vec3(0,0,-1)) };
      IntersectionRay ir =  intersection_ray_plane(r, p);
      if (ir.hit) {
        o->Position = ir.position;
        property_update(v->property, o);
      }
    }
  }

}

static Operation* 
_op_move_object(Object* o, Vec3 start, Vec3 end)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_move_object_do;
  op->undo_cb = operation_move_object_undo;

  Op_Move_Object* omo = calloc(1, sizeof *omo);
  omo->o = o;
  omo->start = start;
  omo->end = end;

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


void 
control_mouse_down(Control* c, Evas_Event_Mouse_Down *e)
{
  if (c->state == MOVE) {
    c->state = IDLE;

    Operation* op = _op_move_object(
          c->view->context->object, //TODO
          c->start,
          c->view->context->object->Position); //TODO

    control_add_operation(c, op);
  }

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
  const Evas_Modifier * mods = e->modifiers;

  if (c->state == IDLE) {
    if (!strcmp(e->keyname, "Escape")) {
      elm_exit();
    } else if ( !strcmp(e->keyname, "g")) {
      Object* o = v->context->object;
      if (o != NULL) {
        //enter move mode
        control_move(c);
      }
    } else if (!strcmp(e->keyname, "z") 
          && evas_key_modifier_is_set(mods, "Control")) {
      control_undo(c);
    } else if (!strcmp(e->keyname, "y")
          && evas_key_modifier_is_set(mods, "Control")) {
      control_redo(c);
    } else if (!strcmp(e->keyname, "f")) {
      Object* o = v->context->object;
      if (o != NULL) {
        //enter move mode
        control_center_camera(c);
      }

    }


  } else if (c->state == MOVE) {
    if (!strcmp(e->keyname, "Escape")) {
      c->state = IDLE;
      Object* o = v->context->object;
      if (o != NULL) {
        o->Position = c->start;
        property_update(v->property, o);
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
  od->o->Position = od->end;
}

void 
operation_move_object_undo(Control* c, void* data)
{
  Op_Move_Object* od = (Op_Move_Object*) data;
  od->o->Position = od->start;
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
  //TODO add an entry in the tree
  tree_add_object(c->view->tree,  od->o);
}

void 
operation_add_object_undo(Control*c, void* data)
{
  Op_Add_Object* od = (Op_Add_Object*) data;
  scene_remove_object(od->s, od->o);
}

