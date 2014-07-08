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
  return c;
}

static Vec3
_objects_center(Eina_List* objects)
{
  Eina_List *l;
  Object *o;
  Vec3 v = vec3_zero();

  EINA_LIST_FOREACH(objects, l, o) {
    v = vec3_add(v, object_world_position_get(o));
  }

  int size = eina_list_count(objects);
  v = vec3_mul(v, 1.0/(float)size);
  return v;
}

static Vec3
_control_move_prepare(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->positions = eina_inarray_new (sizeof(Vec3), size);
  Eina_List *l;
  Object *o;
  Vec3 v = vec3_zero();
  c->dragger_ori = quat_identity();
  EINA_LIST_FOREACH(objects, l, o) {
    Vec3 wp = object_world_position_get(o);
    v = vec3_add(v, wp);
    eina_inarray_push(c->positions, &wp);
    c->dragger_ori = quat_mul(c->dragger_ori, object_world_orientation_get(o));
    /*
    v = vec3_add(v, o->position);
    eina_inarray_push(c->positions, &o->position);
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

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  x = x - cx;
  y = y - cy;

  c->mouse_start = vec2(x,y);
  c->start = _control_move_prepare(c, context_objects_get(v->context));
}

static void
_control_scale_prepare(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->scales = eina_inarray_new (sizeof(Vec3), size);

  Eina_List *l;
  Object *o;
  c->start = vec3_zero();
  int i =0;
  EINA_LIST_FOREACH(objects, l, o) {
    eina_inarray_push(c->scales, &o->scale);
    c->start = vec3_add(c->start, object_world_position_get(o));
    ++i;
  }

  c->start = vec3_mul(c->start, 1.0/ (double)i);

  c->scale_start = vec3(1,1,1);

  c->state = CONTROL_SCALE;
}


static void
_control_scale(Control* c)
{
  View* v = c->view;
  int x, y;
  //evas_pointer_output_xy_get(evas_object_evas_get(v->glview), &x, &y);
  evas_pointer_canvas_xy_get(evas_object_evas_get(v->glview), &x, &y);

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  x = x - cx;
  y = y - cy;

  Object* o = context_object_get(v->context);
  if (o != NULL && c->state != CONTROL_SCALE) {
    _control_scale_prepare(c, context_objects_get(v->context));
    c->mouse_start = vec2(x,y);
  }
}

static void
_control_rotate_prepare(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->quats = eina_inarray_new (sizeof(Quat), size);

  c->dragger_ori = quat_identity();
  c->start = vec3_zero();

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(objects, l, o) {
    eina_inarray_push(c->quats, &o->orientation);
    c->dragger_ori = quat_mul(c->dragger_ori, object_world_orientation_get(o));
    c->start = vec3_add(c->start, object_world_position_get(o));
  }

  c->start = vec3_mul(c->start, 1.0/ eina_list_count(objects));

  c->state = CONTROL_ROTATE;
}


static void
_control_rotate(Control* c)
{
  View* v = c->view;
  int x, y;
  //evas_pointer_output_xy_get(evas_object_evas_get(v->glview), &x, &y);
  evas_pointer_canvas_xy_get(evas_object_evas_get(v->glview), &x, &y);

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  x = x - cx;
  y = y - cy;

  Object* o = context_object_get(v->context);
  if (o != NULL && c->state != CONTROL_ROTATE) {
    _control_rotate_prepare(c, context_objects_get(v->context));
    c->mouse_start = vec2(x,y);
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
    v = quat_rotate_vec3(cam->object->orientation, v);
    v = vec3_add(v, o->position);

    //TODO write a camera_set_position function
    cam->object->position = v;
    camera_recalculate_origin(cam);

  }

}


static void 
_rotate_camera(View* v, float x, float y)
{
  Camera* cam = v->camera->camera_component;
  Object* c = v->camera->object;

  if (vec3_dot(vec3(0,1,0), quat_rotate_vec3(c->orientation, vec3(0,1,0))) <0)
  cam->yaw += 0.005f*x;
  else
  cam->yaw -= 0.005f*x;

  cam->pitch -= 0.005f*y;

  //TODO angles
  Quat qy = quat_angle_axis(cam->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(cam->pitch, vec3(1,0,0));
  Quat result = quat_mul(qy, qp);

  c->angles.x = cam->pitch/M_PI*180.0;
  c->angles.y = cam->yaw/M_PI*180.0;

  Eina_List* objects = context_objects_get(v->context);

  if (eina_list_count(objects) > 0) {
    Vec3 objs_center = _objects_center(objects);
    if (!vec3_equal(objs_center, cam->center)) {
       cam->center = objs_center;
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
  Plane p = { c->start, quat_rotate_vec3(v->camera->object->orientation, vec3(0,0,-1)) };

  if (!vec3_equal(constraint, vec3(1,1,1))) {
    if (constraint.z == 1) {
      p.normal.z = 0;
    }
    if (constraint.y == 1) {
      p.normal.y = 0;
    }
    if (constraint.x == 1) {
      p.normal.x = 0;
    }
  }

  p.normal = vec3_normalized(p.normal);

  Ray rstart = ray_from_screen(v->camera, c->mouse_start.x, c->mouse_start.y, 1);

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  float x = e->cur.canvas.x - cx;
  float y = e->cur.canvas.y - cy;

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
      center = vec3_add(center, o->position);
    }

    if (i>0) center = vec3_mul(center, 1.0f/ (float) i);
    v->context->mos.center =  center;

    if (i == 1)
    control_property_transform_update(c);
  }
}

static void
//_translate_moving_local_axis(Control* c, Evas_Event_Mouse_Move* e, Vec3 axis)
_translate_moving_local_axis(Control* c, Evas_Event_Mouse_Move* e, Vec3 constraint)
{
  View* v = c->view;

  Eina_List* objects = context_objects_get(v->context);
  Vec3 camup = quat_rotate_vec3(v->camera->object->orientation, vec3(0,1,0));
  //printf("dragger ori : %f, %f, %f %f \n ", c->dragger_ori.x, c->dragger_ori.y, c->dragger_ori.z, c->dragger_ori.w);
  Vec3 ca = quat_rotate_vec3(c->dragger_ori, constraint);
  Vec3 cax = quat_rotate_vec3(c->dragger_ori, vec3(constraint.x,0,0));
  Vec3 cay = quat_rotate_vec3(c->dragger_ori, vec3(0,constraint.y,0));
  Vec3 caz = quat_rotate_vec3(c->dragger_ori, vec3(0,0,constraint.z));
  //printf("ca %f, %f, %f \n", ca.x, ca.y, ca.z);
  Vec3 n = vec3_cross(camup, ca);
  if (vec3_equal(constraint, vec3(1,1,0))) n = quat_rotate_vec3(c->dragger_ori, vec3(0,0,1));
  else if (vec3_equal(constraint, vec3(1,0,1))) n = quat_rotate_vec3(c->dragger_ori, vec3(0,1,0));
  else if (vec3_equal(constraint, vec3(0,1,1))) n = quat_rotate_vec3(c->dragger_ori, vec3(1,0,0));
  n = vec3_normalized(n);
  Plane p = { c->start, n };
  //printf("n %f, %f, %f \n", n.x, n.y, n.z);

  if (vec3_equal(constraint, vec3(0,1,0))) {//TODO change this by checking the angle between camup and ca
    Vec3 camright = quat_rotate_vec3(v->camera->object->orientation, vec3(1,0,0));
    p.normal = vec3_cross(camright, ca);
  }

  Ray rstart = ray_from_screen(v->camera, c->mouse_start.x, c->mouse_start.y, 1);

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  float x = e->cur.canvas.x - cx;
  float y = e->cur.canvas.y - cy;

  Ray r = ray_from_screen(v->camera, x, y, 1);

  IntersectionRay ir =  intersection_ray_plane(r, p);
  IntersectionRay irstart =  intersection_ray_plane(rstart, p);

  if (ir.hit && irstart.hit) {
    Vec3 translation = vec3_sub(ir.position, irstart.position);
    //printf("translation %f, %f, %f \n", translation.x, translation.y, translation.z);
    if (vec3_equal(constraint, vec3(1,0,0)) ||
          vec3_equal(constraint, vec3(0,1,0)) ||
          vec3_equal(constraint, vec3(0,0,1))) {
      double dot = vec3_dot(ca, translation);
      translation = vec3_mul(ca, dot);
    }

    Eina_List *l;
    Object *o;
    int i = 0;
    Vec3 center = vec3_zero();
    EINA_LIST_FOREACH(objects, l, o) {
      Vec3* origin = (Vec3*) eina_inarray_nth(c->positions, i);
      Vec3 wordpos = vec3_add(*origin, translation);
      object_world_position_set(o, wordpos);
      ++i;
      center = vec3_add(center, o->position);
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

  Ray rstart = ray_from_screen(v->camera, c->mouse_start.x, c->mouse_start.y, 1);

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  float x = e->cur.canvas.x - cx;
  float y = e->cur.canvas.y - cy;

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
      center = vec3_add(center, o->position);
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

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  float x = e->cur.canvas.x - cx;
  float y = e->cur.canvas.y - cy;

  Vec2 ss = camera_world_to_screen(c->view->camera, c->start);
  Vec2 sss = vec2_sub(c->mouse_start, ss);
  double l1 = vec2_length2(sss);
  Vec2 sd = vec2(x,y); sd = vec2_sub(sd, ss);
  double l2 = vec2_length2(sd);

  double fac = l2/l1;
  double dot = vec2_dot(sss, sd);
  if (dot < 0) fac *= -1;

  c->scale_factor = vec3(fac,fac,fac);
  if (constraint.x == 0) c->scale_factor.x = 1;
  if (constraint.y == 0) c->scale_factor.y = 1;
  if (constraint.z == 0) c->scale_factor.z = 1;

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

  Ray rstart = ray_from_screen(v->camera, c->mouse_start.x, c->mouse_start.y, 1);

  Evas_Coord cx, cy, cw, ch;
  evas_object_geometry_get (v->glview, &cx, &cy, &cw, &ch);
  float x = e->cur.canvas.x - cx;
  float y = e->cur.canvas.y - cy;

  Ray r = ray_from_screen(v->camera, x, y, 1);

  //Vec3 normal = constraint;
  Vec3 normal = quat_rotate_vec3(c->view->camera->object->orientation, vec3(0,0,1));
  Plane p = { c->start, normal };

  IntersectionRay irstart =  intersection_ray_plane(rstart, p);
  IntersectionRay ir =  intersection_ray_plane(r, p);

  Vec3 yos = vec3_normalized(vec3_sub(irstart.position, c->start));
  Vec3 yoe = vec3_normalized(vec3_sub(ir.position, c->start));

  double mdot = vec3_dot(yos, yoe);

  Vec3 cross = vec3_cross(yos,yoe);
  double sign = vec3_dot(normal, cross);
  double angle = acos(mdot);

  Vec3 diff = vec3_sub(c->start, c->view->camera->object->position);
  Vec3 cons;
  if (c->dragger_is_local)
  cons = quat_rotate_vec3(c->dragger_ori, constraint);
  else
  cons = constraint;
  double dotori = vec3_dot(diff, cons);

  if (dotori <0) {
    if (sign > 0) angle *= -1;
  }
  else {
    if (sign < 0) angle *= -1;
  }

  Quat qrot = quat_angle_axis(angle, constraint);
  c->rotation = qrot;

  Eina_List *l;
  Object *o;
  int i = 0;
  EINA_LIST_FOREACH(objects, l, o) {
    Quat* q_origin = (Quat*) eina_inarray_nth(c->quats, i);
    
    if (c->dragger_is_local)
    o->orientation = quat_mul(*q_origin, qrot);
    else
    o->orientation = quat_mul(qrot, *q_origin);
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
    bb.min = vec3_mul(bb.min, d->scale);
    bb.max = vec3_mul(bb.max, d->scale);

    IntersectionRay irtest = { .hit = false };
    if (d->type == DRAGGER_ROTATE) {
      if (d->collider)
      irtest = intersection_ray_mesh(
            r, d->collider,
            dragger->position,
            dragger->orientation, vec3(d->scale,d->scale,d->scale));
    }
    else
    irtest = intersection_ray_box(r, bb, dragger->position, dragger->orientation, vec3(1,1,1));
    if (irtest.hit) {
      if (ir.hit) {
        Vec3 old = vec3_sub(ir.position, r.start);
        Vec3 new = vec3_sub(irtest.position, r.start);
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
  Evas_Coord x, y, w, h;
  evas_object_geometry_get (c->view->glview, &x, &y, &w, &h);
  int ex = e->canvas.x - x;
  int ey = e->canvas.y - y;

  View* v = c->view;
  IntersectionRay ir;
  ir.hit = false;
  Dragger* drag_hit = NULL;

  Eina_List* l;
  Object* dragger;
  EINA_LIST_FOREACH(v->draggers, l, dragger) {
    Dragger* d = object_component_get(dragger, "dragger");
    if (!d) continue;
    if (d->type == DRAGGER_ROTATE)
    dragger_state_set(d, DRAGGER_HIDE);
    else
    dragger_state_set(d, DRAGGER_LOWLIGHT);

    Ray r = ray_from_screen(v->camera, ex, ey, 1000);
    AABox bb = d->box;
    bb.min = vec3_mul(bb.min, d->scale);
    bb.max = vec3_mul(bb.max, d->scale);

    IntersectionRay irtest = { .hit = false };
    if (d->type == DRAGGER_ROTATE) {
      if (d->collider)
      irtest = intersection_ray_mesh(
            r, d->collider,
            dragger->position,
            dragger->orientation, vec3(d->scale,d->scale,d->scale));
    }
    else
    irtest = intersection_ray_box(r, bb, dragger->position, dragger->orientation, vec3(1,1,1));

    if (irtest.hit) {
      if (ir.hit) {
        Vec3 old = vec3_sub(ir.position, r.start);
        Vec3 new = vec3_sub(irtest.position, r.start);
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
    if (drag_hit->type == DRAGGER_ROTATE)
      dragger_state_set(drag_hit, DRAGGER_SHOW_SECOND);
    else
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
  Evas_Coord x, y, w, h;
  evas_object_geometry_get (c->view->glview, &x, &y, &w, &h);
  c->mouse_current.x = e->cur.canvas.x - x;
  c->mouse_current.y = e->cur.canvas.y - y;

  View* v = c->view;
  if (c->state == CONTROL_IDLE) {
    if (e->buttons == 0){
       _draggers_highlight_check(c,c->mouse_current.x, c->mouse_current.y);
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
    //printf("constraint is %f, %f, %f \n", d->constraint.x, d->constraint.y, d->constraint.z);
    if (c->dragger_is_local) {
      /*
      Vec3 normal;
      if (vec3_equal(d->constraint, vec3(0,1,1)))
      normal = quat_rotate_vec3(c->dragger_clicked->orientation, vec3(1,0,0));
      else if (vec3_equal(d->constraint, vec3(1,0,1)))
      normal = quat_rotate_vec3(c->dragger_clicked->orientation, vec3(0,1,0));
      else if (vec3_equal(d->constraint, vec3(1,1,0)))
      normal = quat_rotate_vec3(c->dragger_clicked->orientation, vec3(0,0,1));

      _translate_moving_plane(c,e, 
            quat_rotate_vec3(c->dragger_clicked->orientation, normal));
            */
      _translate_moving_local_axis(c,e, 
            //quat_rotate_vec3(c->dragger_ori, d->constraint));
            d->constraint);
            //vec3(0,1,0));
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
_op_add_object(Scene* s, Eina_List* objects)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_add_object_do;
  op->undo_cb = operation_add_object_undo;

  Op_Add_Object* od = calloc(1, sizeof *od);
  od->s = s;
  od->o = objects;

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
_op_change_property(Component* component, void* data, Property* p, const void* data_old, const void* data_new)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_change_property_do;
  op->undo_cb = operation_change_property_undo;

  Op_Change_Property* od = calloc(1, sizeof *od);
  od->component = component;
  od->data = data;
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
_op_rotate_object(Eina_List* objects, Quat q, bool local)
{
  Operation* op = calloc(1, sizeof *op);

  op->do_cb = operation_rotate_object_do;
  op->undo_cb = operation_rotate_object_undo;

  Op_Rotate_Object* oro = calloc(1, sizeof *oro);
  oro->objects = eina_list_clone(objects);
  oro->quat = q;
  oro->local = local;

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

      Vec3 center = _objects_center(objects);

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

  Evas_Coord x, y, w, h;
  evas_object_geometry_get(v->glview, &x, &y, &w, &h);
  int ex = e->canvas.x - x;
  int ey = e->canvas.y - y;

  if (c->state == CONTROL_DRAGGER_TRANSLATE) {
    c->state = CONTROL_IDLE;

    _draggers_highlight_check(c,ex, ey);

    Eina_List* objects = context_objects_get(v->context);
    Vec3 center = _objects_center(objects);

    Operation* op = _op_move_object(
          objects,
          vec3_sub(center, c->start));

    control_operation_add(c, op);

    return true;
  }
  else if (c->state == CONTROL_DRAGGER_SCALE) {
    c->state = CONTROL_IDLE;

    _draggers_highlight_check(c,ex, ey);

    Eina_List* objects = context_objects_get(c->view->context);

    Operation* op = _op_scale_object(
          objects,
          c->scale_factor);

    control_operation_add(c, op);

    return true;
  }
  else if (c->state == CONTROL_DRAGGER_ROTATE) {
    c->state = CONTROL_IDLE;

    _draggers_highlight_check(c,ex, ey);

    Eina_List* objects = context_objects_get(c->view->context);

    Operation* op = _op_rotate_object(
          objects,
          c->rotation, c->dragger_is_local);

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
    if (!strcmp(e->keyname, "q") 
          && evas_key_modifier_is_set(mods, "Control")) {
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
        o->position = *origin;
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
  Eina_List* l = NULL;
  l = eina_list_append(l, o);

  /*
  Operation* op = _op_add_object(s,o);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
  */
  control_objects_add(c, s, l);
}

void
control_objects_add(Control* c, Scene* s, Eina_List* objects)
{
  Operation* op = _op_add_object(s, objects);
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
control_property_change(Control* c, Component* component, void* data, Property* p, const void* data_old, const void* data_new)
{
  Operation* op = _op_change_property(component, data, p, data_old, data_new);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
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
    property_object_show(c->view->property, o);

}


