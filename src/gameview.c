#include "gameview.h"
#include "glview.h"
#include <dlfcn.h>
#include "component.h"
#include "component/camera.h"
#define __UNUSED__

// Callbacks
static void
_init_gl(Evas_Object *obj)
{
  gl->glEnable(GL_DEPTH_TEST);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glDepthFunc(GL_LEQUAL);
  gl->glClearDepthf(1.0f);
  gl->glClearStencil(0);
}

static void
_del_gl(Evas_Object *obj)
{
  printf("gameview del gl\n");
}

static void
_resize_gl(Evas_Object *obj)
{
  int w, h;
  elm_glview_size_get(obj, &w, &h);

  // GL Viewport stuff. you can avoid doing this if viewport is all the
  // same as last frame if you want
  gl->glViewport(0, 0, w, h);

  GameView* gv = evas_object_data_get(obj, "gameview");

  Scene* s = gv->scene;
  Object* c = s->camera;
  CCamera* cam = object_component_get(c, "camera");
  //ccamera_set_resolution(cam, w, h);
}

static void
_draw_gl(Evas_Object *obj)
{
  int w, h;

  elm_glview_size_get(obj, &w, &h);

  gl->glViewport(0, 0, w, h);
  //gl->glClearColor(1.0,0.8,0.3,1);
  gl->glClearColor(0.2,0.2,0.2,1);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl->glEnable(GL_BLEND);
  gl->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GameView* gv = evas_object_data_get(obj, "gameview");
  scene_update(gv->scene);
  gameview_draw(gv);

  gl->glFinish();
}

static void
_gameview_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
  Ecore_Animator *ani = evas_object_data_get(obj, "ani");
  ecore_animator_del(ani);
  //GameView* gv = evas_object_data_get(obj, "gameview");
}


static void
_set_callbacks(Evas_Object* glview)
{
  Ecore_Animator *ani;
  ani = ecore_animator_add(_anim, glview);
  evas_object_data_set(glview, "ani", ani);

  elm_glview_init_func_set(glview, _init_gl);
  elm_glview_del_func_set(glview, _del_gl);
  elm_glview_resize_func_set(glview, _resize_gl);
  elm_glview_render_func_set(glview, _draw_gl);

  evas_object_event_callback_add(glview, EVAS_CALLBACK_DEL, _gameview_del, glview);
  /*
  evas_object_event_callback_add(glview, EVAS_CALLBACK_KEY_DOWN, _key_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_UP, _mouse_up, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_IN, _mouse_in, NULL);
  */
}


GameView*
create_gameview(Evas_Object *win)
{
  GameView *view = calloc(1,sizeof *view);

  view->box = elm_box_add(win);
  evas_object_size_hint_weight_set(view->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, view->box);
  evas_object_show(view->box);

  view->glview = _create_glview(win);
  elm_box_pack_end(view->box, view->glview);
  evas_object_data_set(view->glview, "gameview", view);
  _set_callbacks(view->glview);

  /*
  //_add_buttons(view, win);
  */

  return view;
}

static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
  GameView* gv = data;
  *gv->window = NULL;
  free(gv);
  component_manager_unload(gv->control->component_manager);
}

Evas_Object* create_gameview_window(View* v, Evas_Object** window, Control* c)
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "gameview");
  elm_win_autodel_set(win, EINA_TRUE);
  GameView* gv = create_gameview(win);
  evas_object_smart_callback_add(win, "delete,request", win_del, gv);
  //Context* cx = v->context;
  Scene* s = v->context->scene;
  gv->scene = s;
  gv->camera = v->camera;
  gv->window = window;
  gv->control = c;

  //component_manager_unload(c->component_manager);
  component_manager_load(c->component_manager); //TODO move this
  
  //evas_object_resize(win, 800/3, 400/3);
  Object* o = s->camera;
  CCamera* cam = object_component_get(o, "camera");
  evas_object_resize(win, cam->width , cam->height);
  evas_object_show(win);
  return win;
}

void 
gameview_draw(GameView* v)
{
  //Render* r = v->render;
  Scene* s = v->scene;
  Object* c = s->camera;
  CCamera* cam = object_component_get(c, "camera");

  Matrix4 cam_mat_inv, mo;

  mat4_inverse(((Object*)c)->matrix, cam_mat_inv);
  Matrix4* projection = &cam->projection;
  Matrix4* ortho = &cam->orthographic;

  //Render just selected to fbo
  Eina_List *l;
  Object *o;

  //Render all objects to fbo to get depth for the lines.
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //gl->glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  //gl->glClearStencil(0);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  gl->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    //mat4_multiply(cam_mat_inv, o->matrix, mo);
    object_draw(o, mo, cam);
  }
  //gl->glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
 
}
