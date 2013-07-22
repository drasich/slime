#include "glview.h"
#include "view.h"
#define __UNUSED__

// Callbacks
void
_init_gl(Evas_Object *obj)
{
  gl = elm_glview_gl_api_get(obj);
  View* v = evas_object_data_get(obj, "view");
  if (v)
  v->render = create_render();

  printf("init gl\n");
  gl->glEnable(GL_DEPTH_TEST);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glDepthFunc(GL_LEQUAL);
  gl->glClearDepthf(1.0f);
  gl->glClearStencil(0);
}

void
_del_gl(Evas_Object *obj)
{
  printf("glview delete gl\n");
  Scene* s = evas_object_data_get(obj, "scene");
  if (s)
  scene_destroy(s);
}

void
_resize_gl(Evas_Object *obj)
{
  gl = elm_glview_gl_api_get(obj);
   int w, h;
   elm_glview_size_get(obj, &w, &h);
   printf("resize gl %d, %d \n", w, h);

   // GL Viewport stuff. you can avoid doing this if viewport is all the
   // same as last frame if you want
   gl->glViewport(0, 0, w, h);
   printf("pan gl %d, %d \n", w, h);

   View* v = evas_object_data_get(obj, "view");
   printf("paff gl %d, %d \n", w, h);
   if (v) {
   camera_set_resolution(v->camera, w, h);
   quad_resize(v->render->quad_outline->mesh, w, h);
   quad_resize(v->render->quad_color->mesh, w, h);

   //TODO
   fbo_resize(v->render->fbo_all, w, h);
   fbo_resize(v->render->fbo_selected, w, h);
   }
}

void
_draw_gl(Evas_Object *obj)
{
  gl = elm_glview_gl_api_get(obj);
   int w, h;
   printf("draw start %p \n", obj);

   elm_glview_size_get(obj, &w, &h);
   if (w!= 888)
   printf("draw 00 gl %d, %d \n", w, h);

   gl->glViewport(0, 0, w, h);
   //gl->glClearColor(1.0,0.8,0.3,1);
   gl->glClearColor(0.2,0.2,0.2,1);
   gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Draw a Triangle
   gl->glEnable(GL_BLEND);
   gl->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //TODO remove the function update here
   Scene* s = evas_object_data_get(obj, "scene");
   if (s)
   scene_update(s);
   else 
   printf("draw gl %d, %d \n", w, h);

   View* v = evas_object_data_get(obj, "view");
   if (v) {
   printf("there is a view in the draw gl %d, %d \n", w, h);
   view_update(v,0);
   view_draw(v);
   }
   else
   printf("draw 10 gl %d, %d \n", w, h);

   gl->glFinish();
}



Eina_Bool
_anim(void *data)
{
   elm_glview_changed_set(data);
   return EINA_TRUE;
}

void
_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
  printf("del ani\n");
   Ecore_Animator *ani = evas_object_data_get(obj, "ani");
   ecore_animator_del(ani);
}

Evas_Object*
_create_glview(Evas_Object* win)
{
  Evas_Object *glview;

  glview = elm_glview_add(win);
  if (!gl)
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
  evas_object_show(glview);
  printf("create glview\n");

  elm_object_focus_set(glview, EINA_TRUE);
  return glview;
}


