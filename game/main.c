#include <Elementary.h>
#include "gameview.h"
#include "scene.h"
#include "resource.h"
#include "texture.h"
#include "component/meshcomponent.h"
#define __UNUSED__

static GameView* _gv;

static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
  printf("win delete \n");
  gameview_destroy(_gv);
  elm_exit();
}

/*
static void
create_window()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("game", "game");
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);

  Evas_Object* box = elm_box_add(win);
  elm_win_resize_object_add(win, box);
  view = create_view(win);
  Evas_Object* glview = view->glview;
  Evas_Object* property = view->property->root;
  Evas_Object* tree = view->tree->root;

  elm_object_part_content_set(hpanes, "left", view->table);
  elm_object_part_content_set(hpanes, "right", property);

  elm_object_part_content_set(panes, "left", tree);
  elm_object_part_content_set(panes, "right", hpanes);

  elm_panes_content_left_size_set(panes, 0.15f);
  elm_panes_content_right_size_set(hpanes, 0.20f);

  //evas_object_resize(win, 800/3, 400/3);
  //evas_object_resize(win, 800, 200);
  evas_object_resize(win, 1200, 400);
  evas_object_show(win);


  //Evas* evas = evas_object_evas_get(win);
  //Evas_Object* edje = create_my_group(evas, "danceoff");
}
*/


static void 
build_scene()
{
  scene_descriptor_init();
  Scene* s = scene_read("scene/scene.eet");
  printf("scene : %p \n", s);
  scene_post_read(s);
  _gv->scene = s;
}

static void
gameviewtest()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "slime");
  evas_object_resize(win, 800, 400);
  evas_object_show(win);
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);
  _gv = create_gameview(win);

}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
  eina_init();
  eet_init();
  s_rm = resource_manager_create();
  resource_read_path(s_rm);
  resource_simple_mesh_create(s_rm);
  resource_shader_create(s_rm);
  resource_texture_create(s_rm);

  s_component_manager = create_component_manager(); //TODO
  component_manager_load(s_component_manager);

  elm_config_preferred_engine_set("opengl_x11");
  //create_window();

  gameviewtest();
  build_scene();

  elm_run();
  elm_shutdown();

  elm_config_preferred_engine_set(NULL);

  return 0;
}
ELM_MAIN()

