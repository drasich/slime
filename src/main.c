#include <Elementary.h>
#define __UNUSED__

void
create_window()
{
   Evas_Object *win;
   win = elm_win_util_standard_add("slime", "slime");
   elm_win_autodel_set(win, EINA_TRUE);

   create_view(win);

   evas_object_resize(win, 800, 480);
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

