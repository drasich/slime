#include <Elementary.h>
#include "view.h"
#include "mesh.h"
#include "object.h"
#include "scene.h"
#include "gl.h"
#define __UNUSED__

static void
_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)event_info;
  //printf("KEY: down, keyname: %s , key %s \n", ev->keyname, ev->key);
  if (!strcmp(ev->keyname, "Escape")) elm_exit();
}


//TODO remove this
static Scene* ss;

//TODO put this data in the view?
static View sview;

// Callbacks
static void
_init_gl(Evas_Object *obj)
{
   ss = create_scene();

   //Object* o = create_object_file("model/smallchar.bin");
   Object* o = create_object_file("model/simpleplane.bin");
   Vec3 t = {0,-5,-8};
   object_set_position(o, t);
   Vec3 axis = {1,0,0};
   Quat q = quat_angle_axis(3.14159f/2.f, axis);
   Vec3 axis2 = {0,0,1};
   Quat q2 = quat_angle_axis(3.14159f/4.f, axis2);
   //q = quat_mul(q, q2);
   //object_set_orientation(o, q);
   scene_add_object(ss,o);

   //object_set_pose(o, "idle");
   //object_set_pose(o, "realwalk");
   object_set_pose(o, "ArmatureAction");

   /*
   Object* yep = create_object();
   //Mesh* mesh2 = create_mesh("model/tex.bin");
   Mesh* mesh2 = create_mesh("model/smallchar.bin");
   object_add_component_mesh(yep, mesh2);
   Vec3 t2 = {10,-5,-20};
   object_set_position(yep, t2);
   scene_add_object(ss,yep);
   */

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
   mat4_set_frustum(sview.projection, -hw*aspect,hw*aspect,-1,1,1,1000.0f);
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

   //TODO remove this function from here
   scene_update(ss);

   scene_draw(ss, w, h);
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

void
create_view(Evas_Object *win)
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

   bt = elm_button_add(win);
   elm_object_text_set(bt, "OK");
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_smart_callback_add(bt, "clicked", _on_done, win);

   mat4_set_frustum(sview.projection, -1,1,-1,1,1,1000.0f);

}


