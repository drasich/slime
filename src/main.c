#include <Elementary.h>
#include "view.h"
#include "gameview.h"
#include "ui/tree.h"
#include "scene.h"
#include "control.h"
#define __UNUSED__

//TODO put these in application
static View* view;

static void
create_workspace(Evas_Object* parent)
{

}

Evas_Object*
create_panes(Evas_Object* win, Eina_Bool hor)
{
  Evas_Object* panes = elm_panes_add(win);
  elm_panes_horizontal_set(panes, hor);
  evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(panes);

  return panes;
}

static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
  printf("win delete \n");
  view_destroy(view);
  elm_exit();
}

static void
create_window()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "slime");
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);

  Evas_Object* panes = create_panes(win, false);
  elm_win_resize_object_add(win, panes);
  Evas_Object* hpanes = create_panes(win, true);
  view = create_view(win);
  Evas_Object* glview = view->glview;
  Evas_Object* property = view->property->root;
  Evas_Object* tree = view->tree->root;


  elm_object_part_content_set(hpanes, "left", property);
  elm_object_part_content_set(hpanes, "right", tree);

  elm_object_part_content_set(panes, "left", view->box);
  elm_object_part_content_set(panes, "right", hpanes);

  elm_panes_content_left_size_set(panes, 0.75f);


  //evas_object_resize(win, 800/3, 400/3);
  //evas_object_resize(win, 800, 100);
  evas_object_resize(win, 1200, 400);
  evas_object_show(win);
}

#include "component/camera.h"
#include "component/line.h"
static void
populate_scene(Control* c, Scene* s)
{
  //Object* o = create_object_file("model/smallchar.bin");
  Object* o = create_object_file("model/cube.bin");
  o->name = eina_stringshare_add("cube");
  //Object* o = create_object_file("model/simpleplane.bin");
  Shader* shader_simple = create_shader("shader/simple.vert", "shader/simple.frag");
  o->mesh->shader = shader_simple;

  Vec3 t = {0,0,0};
  //Vec3 t = {0,0,0};
  object_set_position(o, t);
  //scene_add_object(s,o);
  control_add_object(c, s, o);

  //animation_play(o, "walkquat", LOOP);

  Object* yep = create_object_file("model/smallchar.bin");
  //animation_play(yep, "walkquat", LOOP);
  yep->mesh->shader = shader_simple;

  yep->name = eina_stringshare_add("2222222");
  Vec3 t2 = {-10,0,0};
  object_set_position(yep, t2);
  control_add_object(c,s,yep);

  Object* cam = create_object();
  cam->name = eina_stringshare_add("cameratest");
  Component* compcam = create_component(&camera_desc);
  object_add_component(cam, compcam);

  Component* compline = create_component(&line_desc);
  Line* line = compline->data;
  AABox aabox = { vec3(-1,-1,-1), vec3(1,1,1)};
  line_add_box(line,aabox, vec4(1,1,1,1));
  object_add_component(cam, compline);

  control_add_object(c,s,cam);
  scene_camera_set(s,cam);

  //GLint bits;
  //gl->glGetIntegerv(GL_DEPTH_BITS, &bits);
  //printf("depth buffer %d\n\n", bits);
}


static void 
build_scene()
{
  Scene* s = create_scene();
  evas_object_data_set(view->glview, "scene", s);
  s->view = view;
  view->context->scene = s;
  populate_scene(view->control, s);

}

#include "component/transform.h"
EAPI_MAIN int
elm_main(int argc, char **argv)
{
  elm_config_preferred_engine_set("opengl_x11");
  elm_config_focus_highlight_animate_set(EINA_TRUE);
  elm_config_focus_highlight_enabled_set(EINA_TRUE);
  create_window();

  build_scene();
  test_test();
  //test_display();

  elm_run();
  elm_shutdown();

  elm_config_preferred_engine_set(NULL);

  return 0;
}
ELM_MAIN()

