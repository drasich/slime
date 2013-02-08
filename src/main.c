#include <Elementary.h>
#include "view.h"
#include "ui/property.h"
#define __UNUSED__

void
create_workspace(Evas_Object* parent)
{

}

Evas_Object*
create_panes(Evas_Object* win)
{
  Evas_Object* panes = elm_panes_add(win);
  elm_win_resize_object_add(win, panes);
  //elm_panes_horizontal_set(panes, EINA_TRUE);
  evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(panes);

  return panes;
}

static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
  elm_exit();
}

void
create_window()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "slime");
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);

  Evas_Object* panes = create_panes(win);
  View* view = create_view(win);
  //Evas_Object* view = create_view(win);
  Evas_Object* glview = view->glview;
  Evas_Object* property = property_create(win);

  elm_object_part_content_set(panes, "left", glview);
  elm_object_part_content_set(panes, "right", property);
  //elm_object_part_content_set(panes, "right", hpanes);

  elm_panes_content_left_size_set(panes, 0.75f);

  evas_object_resize(win, 800/3, 400/3);
  evas_object_show(win);
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
  elm_config_preferred_engine_set("opengl_x11");
  create_window();
  elm_config_preferred_engine_set(NULL);

  elm_run();
  elm_shutdown();

  return 0;
}
ELM_MAIN()

