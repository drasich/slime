#include "gameview.h"
#include "glview.h"
#include <dlfcn.h>
#include "component.h"
#include "component/camera.h"
#include "scene.h"
#define __UNUSED__

// Callbacks
static void
_init_gl(Evas_Object *obj)
{
  //Evas_GL_API *gla = elm_glview_gl_api_get(obj);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearDepthf(1.0f);
  glClearStencil(0);

  GameView* gv = evas_object_data_get(obj, "gameview");
  scene_init(gv->scene);
}

static void
_del_gl(Evas_Object *obj)
{
  //printf("gameview del gl\n");
  //TODO delete stuff
}

static void
_resize_gl(Evas_Object *obj)
{
  int w, h;
  elm_glview_size_get(obj, &w, &h);

  // GL Viewport stuff. you can avoid doing this if viewport is all the
  // same as last frame if you want
  glViewport(0, 0, w, h);

  GameView* gv = evas_object_data_get(obj, "gameview");

  Scene* s = gv->scene;
  Object* c = s->camerapointer.object;
  Camera* cam = object_component_get(c, "camera");
  //ccamera_set_resolution(cam, w, h);
}

static void
_draw_gl(Evas_Object *obj)
{
  int w, h;

  elm_glview_size_get(obj, &w, &h);

  glViewport(0, 0, w, h);
  //glClearColor(1.0,0.8,0.3,1);
  glClearColor(0.2,0.2,0.2,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GameView* gv = evas_object_data_get(obj, "gameview");
  scene_update(gv->scene);
  gameview_draw(gv);

  glFinish();
}

static void
_gameview_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
  Ecore_Animator *ani = evas_object_data_get(obj, "ani");
  ecore_animator_del(ani);
  printf("gameview_del\n");

}

void gameview_destroy(GameView* v)
{
  printf("gameview destroy\n");
  component_manager_destroy(v->component_manager);
  scene_del(v->scene);
  free(v);
}

static void
_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)event_info;
  printf("KEY: down, keyname: %s , key %s \n", ev->keyname, ev->key);
  if (!strcmp(ev->keyname, "Escape")) {
    GameView* v = data;
    gameview_destroy(v);
    elm_exit();
  }

}


static void
_set_callbacks(GameView* v)
{
  Evas_Object* glview = v->glview;
  Ecore_Animator *ani;
  ani = ecore_animator_add(_anim, glview);
  evas_object_data_set(glview, "ani", ani);

  elm_glview_init_func_set(glview, _init_gl);
  elm_glview_del_func_set(glview, _del_gl);
  elm_glview_resize_func_set(glview, _resize_gl);
  elm_glview_render_func_set(glview, _draw_gl);

  evas_object_event_callback_add(glview, EVAS_CALLBACK_DEL, _gameview_del, glview);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_KEY_DOWN, _key_down, v);
  /*
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
  _set_callbacks(view);
  //evas_object_show(view->glview);

  /*
  //_add_buttons(view, win);
  */

  return view;
}

static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
  /*
  GameView* gv = data;
  *gv->window = NULL;
  printf("win del will free\n");
  free(gv);
  printf("win del will free is done\n");
  component_manager_unload(gv->control->component_manager);
  printf("win del has unload\n");
  */
}

void 
gameview_draw(GameView* v)
{
  //Render* r = v->render;
  Scene* s = v->scene;
  Object* c = s->camerapointer.object;
  Camera* cam = object_component_get(c, "camera");
  //printf("camera is %s\n", c->name);
  //printf("camera pos.z %f\n", c->Position.Z);

  Matrix4 cam_mat_inv, mo;
  Matrix4 id4;
  mat4_set_identity(id4);

  mat4_inverse(c->matrix, cam_mat_inv);
  Matrix4* projection = &cam->projection;
  Matrix4* ortho = &cam->orthographic;

  //Render just selected to fbo
  Eina_List *l;
  Object *o;

  //Render all objects to fbo to get depth for the lines.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  //glClearStencil(0);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  EINA_LIST_FOREACH(s->objects, l, o) {
    object_draw(o, cam_mat_inv, cam->projection, id4);
  }
  //glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
 
}

