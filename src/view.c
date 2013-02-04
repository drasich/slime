#include <Elementary.h>
#include "view.h"
#include "mesh.h"
#include "object.h"
#include "scene.h"
#include "gl.h"
#include "context.h"
#define __UNUSED__

static void
_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)event_info;
  printf("KEY: down, keyname: %s , key %s \n", ev->keyname, ev->key);
  if (!strcmp(ev->keyname, "Escape")) elm_exit();
}

static float yaw = 0, pitch = 0;

static void
_mouse_move(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event_info;
  //printf("MOUSE: move @ %4i %4i\n", ev->cur.canvas.x, ev->cur.canvas.y);
  //evas_object_move(indicator[0], ev->cur.canvas.x, ev->cur.canvas.y);
  //evas_object_resize(indicator[0], 1, 1);
  //elm_object_focus_set(o, EINA_TRUE);

  if (ev->buttons & 1 == 1){
    Scene* s = evas_object_data_get(o, "scene");
    float x = ev->cur.canvas.x - ev->prev.canvas.x;
    float y = ev->cur.canvas.y - ev->prev.canvas.y;

    const Evas_Modifier * mods = ev->modifiers;
    if (evas_key_modifier_is_set(mods, "Shift")) {
      Vec3 t = {x*0.05f, y*0.05f, 0};
      s->camera->object.Position = vec3_add(s->camera->object.Position, t);
        
    }else {

    Quat q = s->camera->object.Orientation;
    Vec3 axis = {y, -x, 0};
    axis = vec3_normalized(axis);
    Quat rot = quat_angle_axis(0.025f, axis);
    ////q = quat_mul(q, rot); //local
    q = quat_mul(rot,q);//global

    /*
    Vec3 axisx = {1, 0, 0};
    Vec3 axisy = {0, 1, 0};
    pitch += 0.005f*y;
    yaw += -0.005f*x;
    Quat qp = quat_angle_axis(pitch, axisx);
    Quat qy = quat_angle_axis(yaw, axisy);

    q = quat_mul(qy,qp);
    */

    //s->camera->Orientation = q;

    //Vec4 aa = quat_to_axis_angle(q);
    //printf("%f, %f, %f, %f \n", aa.X, aa.Y, aa.Z, aa.W);

    Vec3 at = {0,0,0};
    Vec3 up = {0,1,0};
    s->camera->object.Orientation = quat_lookat(s->camera->object.Position, at, up);

    //Vec4 aa = quat_to_axis_angle(s->camera->Orientation);
    //printf(" quat %f, %f, %f, %f \n", aa.X, aa.Y, aa.Z, aa.W);
    }
  }
}

static void
_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;

  //if (ev->button != 1) return;
  printf("MOUSE: down @ %4i %4i\n", ev->canvas.x, ev->canvas.y);
  //   evas_object_move(indicator[0], ev->canvas.x, ev->canvas.y);
  //  evas_object_resize(indicator[0], 1, 1);
  // evas_object_show(indicator[0]);

  //elm_object_focus_set(o, EINA_TRUE);
  //TODO ray to intersect object
  Scene* s = evas_object_data_get(o, "scene");
  Camera* c = s->camera;
  Vec3 camz = quat_rotate_vec3(quat_inverse(c->object.Orientation), vec3(0,0,-1));
  Vec3 up = {0,1,0};//TODO get from camera
  Vec3 h = vec3_cross(camz, up);
  h = vec3_normalized(h);
  double l = vec3_length(h);
  printf("cam z %f, %f, %f \n", camz.X, camz.Y, camz.Z);
  float vl = tan(M_PI/4.0/2.0) * 1.0; // tan(fov/2)*near
  float aspect = 1.6f; //width/height TODO
  float vh = vl * aspect;

  up = vec3_mul(up, vl);
  h = vec3_mul(h, vh);


}

static void
_mouse_up(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
  //if (ev->button != 1) return;
  printf("MOUSE: up   @ %4i %4i\n", ev->canvas.x, ev->canvas.y);
  //evas_object_hide(indicator[0]);
}

static void
_mouse_wheel(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;

  Scene* s = evas_object_data_get(o, "scene");
  //float x = ev->cur.canvas.x - ev->prev.canvas.x;
  //float y = ev->cur.canvas.y - ev->prev.canvas.y;
  Vec3 axis = {0, 0, ev->z};
  axis = vec3_mul(axis, 0.5f);
  s->camera->object.Position = vec3_add(s->camera->object.Position, axis);
}

// Callbacks
static void
_init_gl(Evas_Object *obj)
{
   Scene* s = create_scene();
   evas_object_data_set(obj, "scene", s);

   Object* o = create_object_file("model/smallchar.bin");
   //Object* o = create_object_file("model/simpleplane.bin");
   Vec3 t = {0,0,0};
   object_set_position(o, t);
   Vec3 axis = {1,0,0};
   Quat q = quat_angle_axis(3.14159f/2.f, axis);
   Vec3 axis2 = {0,0,1};
   Quat q2 = quat_angle_axis(3.14159f/4.f, axis2);
   q = quat_mul(q, q2);
   object_set_orientation(o, q);
   scene_add_object(s,o);

   animation_play(o, "walkquat", LOOP);

   Object* yep = create_object_file("model/smallchar.bin");
   Vec3 t2 = {10,-5,-20};
   object_set_position(yep, t2);
   object_set_orientation(yep, q);
   scene_add_object(s,yep);

   gl->glEnable(GL_DEPTH_TEST);
   gl->glClearDepthf(1.0f);
}

static void
_del_gl(Evas_Object *obj)
{
  /*
   gl->glDeleteShader(gld->vtx_shader);
   gl->glDeleteShader(gld->fgmt_shader);
   gl->glDeleteProgram(gld->program);
   gl->glDeleteBuffers(1, &gld->vbo);

   evas_object_data_del((Evas_Object*)obj, "..gld");
   free(gld);
   */

}

static void
_resize_gl(Evas_Object *obj)
{
   int w, h;
   elm_glview_size_get(obj, &w, &h);

   // GL Viewport stuff. you can avoid doing this if viewport is all the
   // same as last frame if you want
   gl->glViewport(0, 0, w, h);


   float cx = 0;
   float hw = w*0.5f;
   float aspect = (float)w/(float)h;

   /* cx is the eye space center of the zNear plane in X */
   //glFrustum(cx-half_w*aspect, cx+half_w*aspect, bottom, top, zNear, zFar);
   //mat4_set_frustum(sview.projection, -1,1,-1,1,1,1000.0f);
   //TODO not used I think, it is set in object_draw
   //mat4_set_frustum(sview.projection, -hw*aspect,hw*aspect,-1,1,1,1000.0f);
}

static void
_draw_gl(Evas_Object *obj)
{
   int w, h;

   elm_glview_size_get(obj, &w, &h);

   gl->glViewport(0, 0, w, h);
   gl->glClearColor(1.0,0.8,0.3,1);
   gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Draw a Triangle
   gl->glEnable(GL_BLEND);

   //TODO remove the function update here
   Scene* s = evas_object_data_get(obj, "scene");
   scene_update(s);

   scene_draw(s, w, h);
   gl->glFinish();
}

static Eina_Bool
_anim(void *data)
{
   elm_glview_changed_set(data);
   return EINA_TRUE;
}

static void
_on_done(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_del((Evas_Object*)data);
}


static void
_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Ecore_Animator *ani = evas_object_data_get(obj, "ani");
   ecore_animator_del(ani);
}

static Evas_Object*
_create_glview(Evas_Object* win)
{
  Evas_Object *bx, *bt, *glview;
  Ecore_Animator *ani;

  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, bx);
  evas_object_show(bx);

  glview = elm_glview_add(win);
  gl = elm_glview_gl_api_get(glview);
  evas_object_size_hint_align_set(glview, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(glview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_glview_mode_set(glview, ELM_GLVIEW_ALPHA | ELM_GLVIEW_DEPTH);
  elm_glview_resize_policy_set(glview, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
  elm_glview_render_policy_set(glview, ELM_GLVIEW_RENDER_POLICY_ON_DEMAND);
  elm_glview_init_func_set(glview, _init_gl);
  elm_glview_del_func_set(glview, _del_gl);
  elm_glview_resize_func_set(glview, _resize_gl);
  elm_glview_render_func_set(glview, _draw_gl);
  elm_box_pack_end(bx, glview);
  evas_object_show(glview);

  elm_object_focus_set(glview, EINA_TRUE);

  ani = ecore_animator_add(_anim, glview);
  evas_object_data_set(glview, "ani", ani);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_DEL, _del, glview);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_KEY_DOWN, _key_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_UP, _mouse_up, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, NULL);

  /*
  bt = elm_button_add(win);
  elm_object_text_set(bt, "OK");
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
  elm_box_pack_end(bx, bt);
  evas_object_show(bt);
  evas_object_smart_callback_add(bt, "clicked", _on_done, win);
  */

  return glview;

}

View*
create_view(Evas_Object *win)
{
  View *view = malloc(sizeof *view);

  mat4_set_frustum(view->projection, -1,1,-1,1,1,1000.0f);
  view->context = malloc(sizeof *view->context);
  view->glview = _create_glview(win);
  evas_object_data_set(view->glview, "view", view);

  return view;
}

