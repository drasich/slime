#include "control.h"
#include "context.h"
#include "object.h"
#include "view.h"
#include "operation.h"
#include "ui/tree.h"
#include "component/dragger.h"

Control* 
create_control(View* v)
{
  Control* c = calloc(1, sizeof *c);
  c->state = CONTROL_IDLE;
  c->view = v;
  c->redo = NULL;
  //c->shader_simple = create_shader("simple","shader/simple.vert", "shader/simple.frag");
  c->dragger_is_local = true;
  return c;
}

static Vec3
_objects_center(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->positions = eina_inarray_new (sizeof(Vec3), size);
  Eina_List *l;
  Object *o;
  Vec3 v = vec3_zero();
  EINA_LIST_FOREACH(objects, l, o) {
    Vec3 wp = object_world_position_get(o);
    v = vec3_add(v, wp);
    eina_inarray_push(c->positions, &wp);
    /*
    v = vec3_add(v, o->Position);
    eina_inarray_push(c->positions, &o->Position);
    */
  }

  v = vec3_mul(v, 1.0/(float)size);

  return v;
}

static void
_control_move(Control* c)
{
  View* v = c->view;
  int x, y;
  //evas_pointer_output_xy_get(evas_object_evas_get(v->glview), &x, &y);
  evas_pointer_canvas_xy_get(evas_object_evas_get(v->glview), &x, &y);
  Vec2 mousepos = vec2(x,y);
  //printf("mouse pos : %f, %f \n", mousepos.X, mousepos.Y);
  c->start = _objects_center(c, context_objects_get(v->context));
  c->mouse_start = mousepos;

}

static void
_control_scale_prepare(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->scales = eina_inarray_new (sizeof(Vec3), size);

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(objects, l, o) {
    eina_inarray_push(c->scales, &o->scale);
  }

  c->state = CONTROL_SCALE;
}


static void
_control_scale(Control* c)
{
  View* v = c->view;
  int x, y;
  //evas_pointer_output_xy_get(evas_object_evas_get(v->glview), &x, &y);
  evas_pointer_canvas_xy_get(evas_object_evas_get(v->glview), &x, &y);
  Vec2 mousepos = vec2(x,y);
  //printf("mouse pos : %f, %f \n", mousepos.X, mousepos.Y);
  Object* o = context_object_get(v->context);
  if (o != NULL && c->state != CONTROL_SCALE) {
    _control_scale_prepare(c, context_objects_get(v->context));
    c->mouse_start = mousepos;
  }
}

static void
_control_rotate_prepare(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->rotates = eina_inarray_new (sizeof(Vec3), size);

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(objects, l, o) {
    eina_inarray_push(c->rotates, &o->angles);
  }

  c->state = CONTROL_ROTATE;
}


static void
_control_rotate(Control* c)
{
  View* v = c->view;
  int x, y;
  //evas_pointer_output_xy_get(evas_object_evas_get(v->glview), &x, &y);
  evas_pointer_canvas_xy_get(evas_object_evas_get(v->glview), &x, &y);
  Vec2 mousepos = vec2(x,y);
  //printf("mouse pos : %f, %f \n", mousepos.X, mousepos.Y);
  Object* o = context_object_get(v->context);
  if (o != NULL && c->state != CONTROL_ROTATE) {
    _control_rotate_prepare(c, context_objects_get(v->context));
    c->mouse_start = mousepos;
  }

}



static void
_control_center_camera(Control* c)
{
  View* v = c->view;
  Object* o = context_object_get(v->context);
  ViewCamera* cam = v->camera;
  if (o != NULL && c->state == CONTROL_IDLE) {
    //TODO get the distance from the size of the object on the screen
    Vec3 v = vec3(0,0,30);
    v = quat_rotate_vec3(cam->object->Orientation, v);
    v = vec3_add(v, o->Position);

    //TODO write a camera_set_position function
    cam->object->Position = v;
    camera_recalculate_origin(cam);

  }

}


static void 
_rotate_camera(View* v, float x, float y)
{
  Camera* cam = v->camera->camera_component;
  Object* c = v->camera->object;

  cam->yaw += 0.005f*x;
  cam->pitch += 0.005f*y;

  //TODO angles
  Quat qy = quat_angle_axis(cam->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(cam->pitch, vec3(1,0,0));
  Quat result = quat_mul(qy, qp);

  c->angles.X = cam->pitch/M_PI*180.0;
  c->angles.Y = cam->yaw/M_PI*180.0;

  Object* o = context_object_get(v->context);

  if (o != NULL) {
    if (!vec3_equal(o->Position, cam->center)) {
      cam->center = o->Position;
      camera_recalculate_origin(v->camera);
    }
  }

  camera_rotate_around(v->camera, result, cam->center);
}

static void
_translate_moving(Control* c, Evas_Event_Mouse_Move* e, Vec3 constraint)
{
  View* v = c->view;

  Eina_List* objects = context_objects_get(v->context);
  Plane p = { c->start, quat_rotate_vec3(v->camera->object->Orientation, vec3(0,0,-1)) };

  if (constraint.Z == 1) {
    p.Normal.Z = 0;
  }
  else if (constraint.Y == 1) {
    p.Normal.Y = 0;
  }
  else if (constraint.X == 1) {
    p.Normal.X = 0;
  }

  p.Normal = vec3_normalized(p.Normal);

  Ray rstart = ray_from_screen(v->camera, c->mouse_start.X, c->mouse_start.Y, 1);

  float x = e->cur.canvas.x;
  float y = e->cur.canvas.y;
  Ray r = ray_from_screen(v->camera, x, y, 1);

  IntersectionRay ir =  intersection_ray_plane(r, p);
  IntersectionRay irstart =  intersection_ray_plane(rstart, p);

  if (ir.hit && irstart.hit) {
    Vec3 translation = vec3_sub(ir.position, irstart.position);
    translation = vec3_vec3_mul(translation, constraint);
    Eina_List *l;
    Object *o;
    int i = 0;
    Vec3 center = vec3_zero();
    EINA_LIST_FOREACH(objects, l, o) {
      Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
      Vec3 wordpos = vec3_add(*origin, translation);
      object_world_position_set(o, wordpos);
      ++i;
      center = vec3_add(center, o->Position);
    }

    if (i>0) center = vec3_mul(center, 1.0f/ (float) i);
    v->context->mos.center =  center;

    if (i == 1)
    control_property_transform_update(c);
  }
}

static void
_translate_moving_local_axis(Control* c, Evas_Event_Mouse_Move* e, Vec3 axis)
{
  View* v = c->view;

  Eina_List* objects = context_objects_get(v->context);
  Vec3 cam_up = quat_rotate_vec3(v->camera->object->Orientation, vec3(0,1,0));
  axis = vec3_normalized(axis);
  Vec3 pn = vec3_cross(axis, cam_up);
  pn = vec3_normalized(pn);
  Plane p = { c->start, pn };

  Ray rstart = ray_from_screen(v->camera, c->mouse_start.X, c->mouse_start.Y, 1);

  float x = e->cur.canvas.x;
  float y = e->cur.canvas.y;
  Ray r = ray_from_screen(v->camera, x, y, 1);

  IntersectionRay ir =  intersection_ray_plane(r, p);
  IntersectionRay irstart =  intersection_ray_plane(rstart, p);

  if (ir.hit && irstart.hit) {
    Vec3 translation = vec3_sub(ir.position, irstart.position);
    double dot = vec3_dot(axis, translation);
    translation = vec3_mul(axis, dot);
    Eina_List *l;
    Object *o;
    int i = 0;
    Vec3 center = vec3_zero();
    EINA_LIST_FOREACH(objects, l, o) {
      Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
      Vec3 wordpos = vec3_add(*origin, translation);
      object_world_position_set(o, wordpos);
      ++i;
      center = vec3_add(center, o->Position);
    }

    if (i>0) center = vec3_mul(center, 1.0f/ (float) i);
    v->context->mos.center =  center;

    if (i == 1)
    control_property_transform_update(c);
  }
}

static void
_translate_moving_plane(Control* c, Evas_Event_Mouse_Move* e, Vec3 normal)
{
  View* v = c->view;

  Eina_List* objects = context_objects_get(v->context);
  Plane p = { c->start, normal };

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
    int i = 0;
    Vec3 center = vec3_zero();
    EINA_LIST_FOREACH(objects, l, o) {
      Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
      Vec3 wordpos = vec3_add(*origin, translation);
      object_world_position_set(o, wordpos);
      ++i;
      center = vec3_add(center, o->Position);
    }

    if (i>0) center = vec3_mul(center, 1.0f/ (float) i);
    v->context->mos.center =  center;

    if (i == 1)
    control_property_transform_update(c);
  }
}



static void
_scale_moving(Control* c, Evas_Event_Mouse_Move* e, Vec3 constraint)
{
  View* v = c->view;

  Eina_List* objects = context_objects_get(v->context);

  float x = e->cur.canvas.x;
  float y = e->cur.canvas.y;

  //TODO scale better (don't start from 0)
  Vec2 d = vec2(x - c->mouse_start.X, y - c->mouse_start.Y);
  double s = vec2_length(d) * 0.1f;
  c->scale_factor = vec3(s,s,s);
  if (constraint.X == 0) c->scale_factor.X = 1;
  if (constraint.Y == 0) c->scale_factor.Y = 1;
  if (constraint.Z == 0) c->scale_factor.Z = 1;

  Eina_List *l;
  Object *o;
  int i = 0;
  EINA_LIST_FOREACH(objects, l, o) {
    Vec3* scale_origin = (Vec3*) eina_inarray_nth(c->scales, i);
    //o->scale = vec3_mul(*scale_origin, c->scale_factor);
    o->scale = vec3_vec3_mul(*scale_origin, c->scale_factor);
    ++i;
  }

  if (i == 1)
  control_property_transform_update(c);
}

static void
_rotate_moving(Control* c, Evas_Event_Mouse_Move* e, Vec3 constraint)
{
  View* v = c->view;

  Eina_List* objects = context_objects_get(v->context);

  float x = e->cur.canvas.x;
  float y = e->cur.canvas.y;

  Vec2 d = vec2(x - c->mouse_start.X, y - c->mouse_start.Y);
  double s = vec2_length(d);
  /*
  c->scale_factor = vec3(s,s,s);
  if (constraint.X == 0) c->scale_factor.X = 0;
  if (constraint.Y == 0) c->scale_factor.Y = 0;
  if (constraint.Z == 0) c->scale_factor.Z = 0;
  */
  c->scale_factor = vec3_mul(constraint,s);

  Eina_List *l;
  Object *o;
  int i = 0;
  EINA_LIST_FOREACH(objects, l, o) {
    Vec3* angles_origin = (Vec3*) eina_inarray_nth(c->rotates, i);
    //o->scale = vec3_mul(*scale_origin, c->scale_factor);
    o->angles = vec3_add(*angles_origin, c->scale_factor);
    ++i;
  }

  if (i == 1)
  control_property_transform_update(c);
}


static void
_draggers_highlight_check(Control* c, Evas_Coord x, Evas_Coord y)
{
  View* v = c->view;
  IntersectionRay ir;
  ir.hit = false;
  Dragger* drag_hit = NULL;

  Eina_List* l;
  Object* dragger;
  EINA_LIST_FOREACH(v->draggers, l, dragger) {
    Dragger* d = object_component_get(dragger, "dragger");
    if (!d) continue;
    dragger_state_set(d, DRAGGER_IDLE);

    Ray r = ray_from_screen(v->camera, x, y, 1000);
    AABox bb = d->box;
    bb.Min = vec3_mul(bb.Min, d->scale);
    bb.Max = vec3_mul(bb.Max, d->scale);

    IntersectionRay irtest = { .hit = false };
    if (d->type == DRAGGER_ROTATE) {
      if (d->collider)
      irtest = intersection_ray_mesh(
            r, d->collider,
            dragger->Position,
            dragger->Orientation, vec3(d->scale,d->scale,d->scale));
    }
    else
    irtest = intersection_ray_box(r, bb, dragger->Position, dragger->Orientation, vec3(1,1,1));
    if (irtest.hit) {
      if (ir.hit) {
        Vec3 old = vec3_sub(ir.position, r.Start);
        Vec3 new = vec3_sub(irtest.position, r.Start);
        if (vec3_length2(new) < vec3_length2(old)) {
          ir = irtest;
          drag_hit = d;
        }
      }
      else {
        ir = irtest;
        drag_hit = d;
      }
    }
  }
  
  if (ir.hit && drag_hit) {
    dragger_state_set(drag_hit, DRAGGER_HIGHLIGHT);
  }
}

static bool
_draggers_click_check(Control* c, Evas_Event_Mouse_Down* e)
{
  View* v = c->view;
  IntersectionRay ir;
  ir.hit = false;
  Dragger* drag_hit = NULL;

  Eina_List* l;
  Object* dragger;
  EINA_LIST_FOREACH(v->draggers, l, dragger) {
    Dragger* d = object_component_get(dragger, "dragger");
    if (!d) continue;
    dragger_state_set(d, DRAGGER_HIDE);

    Ray r = ray_from_screen(v->camera, e->canvas.x, e->canvas.y, 1000);
    AABox bb = d->box;
    bb.Min = vec3_mul(bb.Min, d->scale);
    bb.Max = vec3_mul(bb.Max, d->scale);

    IntersectionRay irtest = { .hit = false };
    if (d->type == DRAGGER_ROTATE) {
      if (d->collider)
      irtest = intersection_ray_mesh(
            r, d->collider,
            dragger->Position,
            dragger->Orientation, vec3(d->scale,d->scale,d->scale));
    }
    else
    irtest = intersection_ray_box(r, bb, dragger->Position, dragger->Orientation, vec3(1,1,1));

    if (irtest.hit) {
      if (ir.hit) {
        Vec3 old = vec3_sub(ir.position, r.Start);
        Vec3 new = vec3_sub(irtest.position, r.Start);
        if (vec3_length2(new) < vec3_length2(old)) {
          ir = irtest;
          drag_hit = d;
          c->dragger_clicked =  dragger;
        }
      }
      else {
        ir = irtest;
        drag_hit = d;
        c->dragger_clicked =  dragger;
      }
    }
  }
  
  if (ir.hit && drag_hit) {
      dragger_state_set(drag_hit, DRAGGER_SELECTED);
      if (drag_hit->type == DRAGGER_TRANSLATE) {
        _control_move(c);
        c->state = CONTROL_DRAGGER_TRANSLATE;
      }
      else if (drag_hit->type == DRAGGER_SCALE) {
        _control_scale(c);
        c->state = CONTROL_DRAGGER_SCALE;
      }
      else if (drag_hit->type == DRAGGER_ROTATE) {
        _control_rotate(c);
        c->state = CONTROL_DRAGGER_ROTATE;
      }
      return true;
  }

  return false;
}


void
control_mouse_move(Control* c, Evas_Event_Mouse_Move *e)
{
  c->mouse_current.X = e->cur.canvas.x;
  c->mouse_current.Y = e->cur.canvas.y;

  View* v = c->view;
  if (c->state == CONTROL_IDLE) {
    if (e->buttons == 0){
       _draggers_highlight_check(c,e->cur.canvas.x, e->cur.canvas.y);
    }
    else if ( (e->buttons & 1) == 1){
      float x = e->cur.canvas.x - e->prev.canvas.x;
      float y = e->cur.canvas.y - e->prev.canvas.y;

      const Evas_Modifier * mods = e->modifiers;
      if (evas_key_modifier_is_set(mods, "Shift")) {
        Vec3 t = {-x*0.05f, y*0.05f, 0};
        camera_pan(v->camera, t);
      } else {
        _rotate_camera(v, x, y);
      }
    }
  } else if (c->state == CONTROL_MOVE) {
    _translate_moving(c,e, vec3(1,1,1));
  } else if (c->state == CONTROL_SCALE) {
    _scale_moving(c,e, vec3(1,1,1));
  }
  else if (c->state == CONTROL_DRAGGER_TRANSLATE) {
    Dragger* d = object_component_get(c->dragger_clicked, "dragger");
    //printf("constraint is %f, %f, %f \n", d->constraint.X, d->constraint.Y, d->constraint.Z);
    if (c->dragger_is_local) {
      /*
      Vec3 normal;
      if (vec3_equal(d->constraint, vec3(0,1,1)))
      normal = quat_rotate_vec3(c->dragger_clicked->Orientation, vec3(1,0,0));
      else if (vec3_equal(d->constraint, vec3(1,0,1)))
      normal = quat_rotate_vec3(c->dragger_clicked->Orientation, vec3(0,1,0));
      else if (vec3_equal(d->constraint, vec3(1,1,0)))
      normal = quat_rotate_vec3(c->dragger_clicked->Orientation, vec3(0,0,1));

      _translate_moving_plane(c,e, 
            quat_rotate_vec3(c->dragger_clicked->Orientation, normal));
            */
      _translate_moving_local_axis(c,e, 
            quat_rotate_vec3(c->dragger_clicked->Orientation, vec3(0,0,1)));
    }
    else {
      Vec3 constraint = d->constraint;
      _translate_moving(c,e, constraint);
    }
    
  }
  else if (c->state == CONTROL_DRAGGER_SCALE) {
    Dragger* d = object_component_get(c->dragger_clicked, "dragger");
    Vec3 constraint = d->constraint;
    _scale_moving(c,e, constraint);
  }
  else if (c->state == CONTROL_DRAGGER_ROTATE) {
    Dragger* d = object_component_get(c->dragger_clicked, "dragger");
    Vec3 constraint = d->constraint;
    _rotate_moving(c,e, constraint);
  }

}

static Operation* 
_op_move_object(Eina_List* objects, Vec3 translation)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_move_object_do;
  op->undo_cb = operation_move_object_undo;

  Op_Move_Object* omo = calloc(1, sizeof *omo);
  omo->objects = eina_list_clone(objects);
  omo->translation = translation;

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

static Operation* 
_op_change_property(Component* component, Property* p, const void* data_old, const void* data_new)
{
  //TODO handle the case when data is not a pointer and we have to save the data..
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_change_property_do;
  op->undo_cb = operation_change_property_undo;

  Op_Change_Property* od = calloc(1, sizeof *od);
  od->component = component;
  od->p = p;
  od->value_old = data_old;
  od->value_new = data_new;

  op->data = od;
  return op;
}

static Operation* 
_op_object_add_component(Object* o, Component* c)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_object_add_component_do;
  op->undo_cb = operation_object_add_component_undo;

  Op_Object_Add_Component* od = calloc(1, sizeof *od);
  od->o = o;
  od->c = c;

  op->data = od;
  return op;
}

static Operation* 
_op_object_remove_component(Object* o, Component* c)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_object_add_component_undo;
  op->undo_cb = operation_object_add_component_do;

  Op_Object_Add_Component* od = calloc(1, sizeof *od);
  od->o = o;
  od->c = c;

  op->data = od;
  return op;
}


static Operation* 
_op_scale_object(Eina_List* objects, Vec3 scale)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_scale_object_do;
  op->undo_cb = operation_scale_object_undo;

  Op_Scale_Object* oso = calloc(1, sizeof *oso);
  oso->objects = eina_list_clone(objects);
  oso->scale = scale;

  op->data = oso;

  return op;
}

static Operation* 
_op_rotate_object(Eina_List* objects, Vec3 rotate)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_rotate_object_do;
  op->undo_cb = operation_rotate_object_undo;

  Op_Rotate_Object* oro = calloc(1, sizeof *oro);
  oro->objects = eina_list_clone(objects);
  oro->angle = rotate;

  op->data = oro;

  return op;
}


bool
control_mouse_down(Control* c, Evas_Event_Mouse_Down *e)
{
  View* v = c->view;

  if (c->state == CONTROL_IDLE) {
    if (e->button == 1 && 
          context_object_get(c->view->context) &&
          _draggers_click_check(c, e)) 
    return true;
  }
  else if (c->state == CONTROL_MOVE) {
    if (e->button == 1) {
      c->state = CONTROL_IDLE;

      Eina_List* objects = context_objects_get(c->view->context);

      Vec3 center = _objects_center(c, objects);

      Operation* op = _op_move_object(
            objects,
            vec3_sub(center, c->start));

      control_operation_add(c, op);
      return true;
    }
  }
  else if (c->state == CONTROL_SCALE) {
    if (e->button == 1) {
      c->state = CONTROL_IDLE;

      Eina_List* objects = context_objects_get(c->view->context);

      Operation* op = _op_scale_object(
            objects,
            c->scale_factor);

      control_operation_add(c, op);
      return true;
    }
  }

  return false;
}

bool
control_mouse_up(Control* c, Evas_Event_Mouse_Up *e)
{
  View* v = c->view;

  if (c->state == CONTROL_DRAGGER_TRANSLATE) {
    c->state = CONTROL_IDLE;

    _draggers_highlight_check(c,e->canvas.x, e->canvas.y);

    Eina_List* objects = context_objects_get(v->context);
    Vec3 center = _objects_center(c, objects);

    Operation* op = _op_move_object(
          objects,
          vec3_sub(center, c->start));

    control_operation_add(c, op);

    return true;
  }
  else if (c->state == CONTROL_DRAGGER_SCALE) {
    c->state = CONTROL_IDLE;

    _draggers_highlight_check(c,e->canvas.x, e->canvas.y);

    Eina_List* objects = context_objects_get(c->view->context);

    Operation* op = _op_scale_object(
          objects,
          c->scale_factor);

    control_operation_add(c, op);

    return true;
  }
  else if (c->state == CONTROL_DRAGGER_ROTATE) {
    c->state = CONTROL_IDLE;

    _draggers_highlight_check(c,e->canvas.x, e->canvas.y);

    Eina_List* objects = context_objects_get(c->view->context);

    Operation* op = _op_rotate_object(
          objects,
          c->scale_factor);

    control_operation_add(c, op);
    return true;
  }


  return false;
}


void 
control_key_down(Control* c, Evas_Event_Key_Down *e)
{
  View* v = c->view;
  Scene* s = v->context->scene;
  Object* o = context_object_get(v->context);
  Eina_List* objects = context_objects_get(v->context);

  const Evas_Modifier * mods = e->modifiers;

  if (c->state == CONTROL_IDLE) {
    if (!strcmp(e->keyname, "Escape")) {
      view_destroy(c->view);
      elm_exit();
    } else if (!strcmp(e->keyname, "g") && o!= NULL) {
      _control_move(c);
      c->state = CONTROL_MOVE;
    } else if ( !strcmp(e->keyname, "s")) {
      _control_scale(c);
    } else if (!strcmp(e->keyname, "z") 
          && evas_key_modifier_is_set(mods, "Control")) {
      control_undo(c);
    } else if (!strcmp(e->keyname, "y")
          && evas_key_modifier_is_set(mods, "Control")) {
      control_redo(c);
    } else if (!strcmp(e->keyname, "f")) {
      if (o != NULL) {
        _control_center_camera(c);
      }
    } else if (!strcmp(e->keyname, "x")) {
      if (o != NULL) {
        control_remove_object(c, s, objects);
      }
    } else if (!strcmp(e->keyname, "a")) {
      context_objects_clean(v->context);
    } else if (!strcmp(e->keyname, "space")) {
      if (c->view->draggers == c->view->dragger_translate)
        c->view->draggers = c->view->dragger_rotate;
      else if (c->view->draggers == c->view->dragger_rotate)
        c->view->draggers = c->view->dragger_scale;
      else if (c->view->draggers == c->view->dragger_scale)
        c->view->draggers = c->view->dragger_translate;
    }

  } else if (c->state == CONTROL_MOVE) {
    if (!strcmp(e->keyname, "Escape")) {
      c->state = CONTROL_IDLE;

      // put back the objects to original position
      Eina_List *l;
      Object *o;
      int i = 0;
      EINA_LIST_FOREACH(objects, l, o) {
        Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
        o->Position = *origin;
        i++;
      }

      if (i == 1)
      control_property_transform_update(c);

    }
  } else if (c->state == CONTROL_SCALE) {
    if (!strcmp(e->keyname, "Escape")) {
      c->state = CONTROL_IDLE;

      Eina_List *l;
      Object *o;
      int i = 0;
      EINA_LIST_FOREACH(objects, l, o) {
        Vec3* scale_origin = (Vec3*) eina_inarray_nth(c->scales, i);
        o->scale = *scale_origin;
        i++;
      }

      if (i == 1)
      control_property_transform_update(c);

    }
  }

}

void 
control_operation_add(Control* c, Operation* op)
{
  c->undo = eina_list_append(c->undo, op);
  control_redo_clean(c);
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
control_redo_clean(Control* c)
{
  Operation *op;

  EINA_LIST_FREE(c->redo, op ) {
    //TODO might have to do a special callback for cleaning the operation
    free(op->data);
    free(op);
  }

}


void
control_object_add(Control* c, Scene* s, Object* o)
{
  Operation* op = _op_add_object(s,o);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
}

void
control_remove_object(Control* c, Scene* s, Eina_List* objects)
{
  Operation* op = _op_remove_object(s,objects);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
}

void
control_property_change(Control* c, Component* component, Property* p, const void* data_old, const void* data_new)
{
  Operation* op = _op_change_property(component, p, data_old, data_new);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
  if (p->type == PROPERTY_POINTER)
   {
    const Object* old = data_old;
    const Object* new = data_new;
   }
}


void
control_property_update(Control* c, Component* component)
{
  if (component->object == c->view->context->object) //TODO if display somewhere
  property_update_components_data(c->view->property, component);

  if (!strcmp(component->name, "object"))
  tree_object_update(c->view->tree, component->object);
}

void
control_property_transform_update(Control* c)
{
  property_update_component(c->view->property, "object");
}


void
control_component_add(Control* c, Object* o, Component* comp)
{
  Operation* op = _op_object_add_component(o,comp);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
}

void
control_component_remove(Control* c, Object* o, Component* comp)
{
  Operation* op = _op_object_remove_component(o,comp);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
}

void control_on_object_components_changed(Control* c, Object* o)
{
  if (context_object_get(c->view->context) == o)
    property_object_display(c->view->property, o);

}


