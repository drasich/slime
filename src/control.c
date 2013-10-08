#include "control.h"
#include "context.h"
#include "object.h"
#include "view.h"
#include "operation.h"
#include "ui/tree.h"

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
_objects_center(Control* c, Eina_List* objects)
{
  int size = eina_list_count(objects);
  c->positions = eina_inarray_new (sizeof(Vec3), size);
  Eina_List *l;
  Object *o;
  Vec3 v = vec3_zero();
  EINA_LIST_FOREACH(objects, l, o) {
    v = vec3_add(v, o->Position);
    eina_inarray_push(c->positions, &o->Position);
  }

  v = vec3_mul(v, 1.0/(float)size);

  return v;
}

static void
_control_move_prepare(Control* c)
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
_scale_prepare(Control* c, Eina_List* objects)
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
    _scale_prepare(c, context_objects_get(v->context));
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
      o->Position = vec3_add(*origin, translation);
      ++i;
      center = vec3_add(center, o->Position);
    }
    //v->dragger->Position = vec3_add(c->start, translation);

    if (i>0) center = vec3_mul(center, 1.0f/ (float) i);
    v->context->mos.center =  center;

    if (i == 1)
    control_property_update_transform(c);
  }
}

#include "component/dragger.h"
void
control_mouse_move(Control* c, Evas_Event_Mouse_Move *e)
{
  c->mouse_current.X = e->cur.canvas.x;
  c->mouse_current.Y = e->cur.canvas.y;

  View* v = c->view;
  if (c->state == CONTROL_IDLE) {
    if (e->buttons == 0){
      Object* od = v->dragger;
      Dragger* d = object_component_get(od, "dragger");
      if (!d) return;

      Ray r = ray_from_screen(v->camera, e->cur.canvas.x, e->cur.canvas.y, 1000);
      AABox bb = d->box;
      bb.Min = vec3_mul(bb.Min, d->scale);
      bb.Max = vec3_mul(bb.Max, d->scale);

      IntersectionRay ir = intersection_ray_box(r, bb, od->Position, od->Orientation, vec3(1,1,1));
    
      //dragger_highlight_set(d, ir.hit);
      if (ir.hit)
      dragger_state_set(d, DRAGGER_HIGHLIGHT);
      else
      dragger_state_set(d, DRAGGER_IDLE);
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
    Eina_List* objects = context_objects_get(v->context);

    float x = e->cur.canvas.x;
    float y = e->cur.canvas.y;

    Vec2 d = vec2(x - c->mouse_start.X, y - c->mouse_start.Y);
    c->scale_factor = vec2_length(d) * 0.1f;

    Eina_List *l;
    Object *o;
    int i = 0;
    EINA_LIST_FOREACH(objects, l, o) {
      Vec3* scale_origin = (Vec3*) eina_inarray_nth(c->scales, i);
      o->scale = vec3_mul(*scale_origin, c->scale_factor);
      ++i;
    }

    if (i == 1)
    control_property_update_transform(c);
  }
  else if (c->state == CONTROL_DRAGGER) {
    _translate_moving(c,e, vec3(0,0,1));
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

bool
control_mouse_down(Control* c, Evas_Event_Mouse_Down *e)
{
  View* v = c->view;

  if (c->state == CONTROL_IDLE) {
    Object* od = v->dragger;
    Dragger* d = object_component_get(od, "dragger");
    if (!d) return;

    Ray r = ray_from_screen(v->camera, e->canvas.x, e->canvas.y, 1000);
    AABox bb = d->box;
    bb.Min = vec3_mul(bb.Min, d->scale);
    bb.Max = vec3_mul(bb.Max, d->scale);

    IntersectionRay ir = intersection_ray_box(r, bb, od->Position, od->Orientation, vec3(1,1,1));

    if (ir.hit) {
      dragger_state_set(d, DRAGGER_SELECTED);
      _control_move_prepare(c);
      c->state = CONTROL_DRAGGER;
      return true;
    }
  }
  else if (c->state == CONTROL_MOVE) {
    c->state = CONTROL_IDLE;

    Eina_List* objects = context_objects_get(c->view->context);

    Vec3 center = _objects_center(c, objects);

    Operation* op = _op_move_object(
          objects,
          vec3_sub(center, c->start));

    control_operation_add(c, op);
    return true;
  }
  else if (c->state == CONTROL_SCALE) {
    c->state = CONTROL_IDLE;

    //TODO
    Eina_List* objects = context_objects_get(c->view->context);

    Operation* op = _op_scale_object(
          objects,
          vec3(c->scale_factor, c->scale_factor, c->scale_factor));

    control_operation_add(c, op);
    return true;
  }

  return false;
}

bool
control_mouse_up(Control* c, Evas_Event_Mouse_Up *e)
{
  View* v = c->view;

  if (c->state == CONTROL_DRAGGER) {
    c->state = CONTROL_IDLE;

    Object* od = v->dragger;
    Dragger* d = object_component_get(od, "dragger");
    if (!d) return;

    Ray r = ray_from_screen(v->camera, e->canvas.x, e->canvas.y, 1000);
    AABox bb = d->box;
    bb.Min = vec3_mul(bb.Min, d->scale);
    bb.Max = vec3_mul(bb.Max, d->scale);

    IntersectionRay ir = intersection_ray_box(r, bb, od->Position, od->Orientation, vec3(1,1,1));

    if (ir.hit)
    dragger_state_set(d, DRAGGER_HIGHLIGHT);
    else
    dragger_state_set(d, DRAGGER_IDLE);

    Eina_List* objects = context_objects_get(v->context);

    Vec3 center = _objects_center(c, objects);

    Operation* op = _op_move_object(
          objects,
          vec3_sub(center, c->start));

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
      _control_move_prepare(c);
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
      context_clean_objects(v->context);
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
      control_property_update_transform(c);

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
      control_property_update_transform(c);

    }
  }

}

void 
control_operation_add(Control* c, Operation* op)
{
  c->undo = eina_list_append(c->undo, op);
  control_clean_redo(c);
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
control_add_object(Control* c, Scene* s, Object* o)
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
control_change_property(Control* c, Component* component, Property* p, const void* data_old, const void* data_new)
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
  tree_update_object(c->view->tree, component->object);
}

void
control_property_update_transform(Control* c)
{
  property_update_component(c->view->property, "object");
}


void
control_object_add_component(Control* c, Object* o, Component* comp)
{
  Operation* op = _op_object_add_component(o,comp);
  control_operation_add(c, op);
  op->do_cb(c, op->data);
}

void
control_object_remove_component(Control* c, Object* o, Component* comp)
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


