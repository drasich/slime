#include <Elementary.h>
#include "view.h"
#define __UNUSED__

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

   create_view(win);

   evas_object_resize(win, 800/3, 400/3);
   evas_object_show(win);
}

EAPI_MAIN int
elm_main(int    argc,
      char **argv)
{
  elm_config_preferred_engine_set("opengl_x11");
  create_window();
  elm_config_preferred_engine_set(NULL);

  elm_run();
  elm_shutdown();

  return 0;
}
ELM_MAIN()

