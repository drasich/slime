#include "gameview.h"
#include "glview.h"

GameView*
create_gameview(Evas_Object *win)
{
  GameView *view = calloc(1,sizeof *view);

  view->box = elm_box_add(win);
  evas_object_size_hint_weight_set(view->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, view->box);
  evas_object_show(view->box);

  view->glview = _create_glview(win);
  elm_box_pack_end(view->box, view->glview);
  //_set_callbacks(view->glview);

  //_add_buttons(view, win);

  evas_object_data_set(view->glview, "view", view);

  return view;
}


Evas_Object* create_gameview_window()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "gameview");
  elm_win_autodel_set(win, EINA_TRUE);
  //evas_object_smart_callback_add(win, "delete,request", win_del, NULL);
  GameView* gv = create_gameview(win);
  
  evas_object_resize(win, 800, 400);
  evas_object_show(win);
  return win;
}
