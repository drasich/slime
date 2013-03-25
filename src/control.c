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

static void rotate_camera(View* v, float x, float y)
{
  Camera* cam = v->camera;
  Object* c = (Object*) cam;

  cam->yaw += 0.005f*x;
  cam->pitch += 0.005f*y;

  Quat qy = quat_angle_axis(cam->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(cam->pitch, vec3(1,0,0));
  Quat result = quat_mul(qy, qp);

  c->Orientation = result;

  Object* o = v->context->object;
  if (o == NULL) return;

  camera_rotate_around(cam, result, o->Position);
}


void
control_mouse_move(Control* c, Evas_Event_Mouse_Move *e)
{
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
      }
    }
  }

}

void 
control_mouse_down(Control* c, Evas_Event_Mouse_Down *e)
{
  if (c->state == MOVE) {
    c->state = IDLE;
  }

}

void 
control_key_down(Control* c, Evas_Event_Key_Down *e)
{
  View* v = c->view;

  if (c->state == IDLE) {
    if (!strcmp(e->keyname, "Escape")) {
      elm_exit();
    } else if ( !strcmp(e->keyname, "g")) {
      Object* o = v->context->object;
      if (o != NULL) {
        //enter move mode
        control_move(c);
      }
    }
  } else if (c->state == MOVE) {
    if (!strcmp(e->keyname, "Escape")) {
      c->state = IDLE;
      Object* o = v->context->object;
      if (o != NULL) {
        o->Position = c->start;
      }
    }

  }
}
