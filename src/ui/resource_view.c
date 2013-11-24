#include "ui/resource_view.h"
#include "resource.h"
#define __UNUSED__

static char*
gl_scene_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
  char buf[256];
  Scene* s = (Scene*) data;
  snprintf(buf, sizeof(buf), "%s", s->name);
  return strdup(buf);
}


static void
_gl_double_clicked(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   printf("double clicked: %p\n", event_info);
}

static void
_gl_pressed(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   printf("clicked: %p\n", event_info);
}

static void
_bt_close(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *notify = data;
   evas_object_hide(notify);
}

static Evas_Object* _notify = NULL;
static void
_display_notify(Evas_Object* win)
{
  if (_notify) {
    evas_object_show(_notify);
    return;
  }

  Evas_Object* notify, *bx, *lb, *bt;

  notify = elm_notify_add(win);
  _notify = notify;
  evas_object_size_hint_weight_set(notify, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_notify_align_set(notify, 0.0, 0.0);
  evas_object_show(notify);
  elm_notify_timeout_set(notify, 3);

  bx = elm_box_add(win);
  elm_object_content_set(notify, bx);
  elm_box_horizontal_set(bx, EINA_TRUE);
  evas_object_show(bx);

  lb = elm_label_add(win);
  elm_object_text_set(lb, "You cannot remove the last scene.");
  elm_box_pack_end(bx, lb);
  evas_object_show(lb);

  /*
  bt = elm_button_add(win);
  elm_object_text_set(bt, "close");
  evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
  elm_box_pack_end(bx, bt);
  evas_object_show(bt);
  */

  /*
  bt = elm_button_add(win);
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_object_text_set(bt, "Top Left");
  evas_object_smart_callback_add(bt, "clicked", _bt, notify);
  elm_table_pack(tb, bt, 1, 1, 1, 1);
  evas_object_show(bt);
  */

}

static void
_scene_remove(void *data,
      Evas_Object *obj,
      void *event_info)
{
  ResourceView* rv = evas_object_data_get(obj, "resourceview");
  Elm_Object_Item* item = data;
  Scene* s = elm_object_item_data_get(item);
  printf("remove the scene: %s \n", s->name);

  Elm_Object_Item* another = elm_genlist_item_next_get(item);
  if (!another) {
    another = elm_genlist_item_prev_get(item);
  }

  if (!another) {
    //display error
    //Evas* e = evas_object_evas_get(obj);
    //Evas_Object* win = evas_object_top_get(e);
    _display_notify(rv->gl);
    return;
  }

  Scene* ns = elm_object_item_data_get(another);

  view_scene_set(rv->view, ns);

  elm_object_item_del(item);
  resource_scene_del(s_rm, s);
}

static void
_scene_add(void *data,
      Evas_Object *obj,
      void *event_info)
{
  ResourceView* rv = evas_object_data_get(obj, "resourceview");
  Elm_Object_Item* item = data;

  Scene* s = scene_new();

  view_scene_set(rv->view, s);
  resource_view_scene_add(rv, s);
}

static Evas_Object*
_create_scene_menu(Evas_Object* win, Elm_Object_Item* item)
{
  Evas_Object* menu;

  menu = elm_menu_add(win);

  elm_menu_item_add(menu, NULL, NULL, "remove", _scene_remove, item);
  elm_menu_item_add(menu, NULL, NULL, "add new scene", _scene_add, item);

  return menu;
}


static void
_gl_longpress(void *data, Evas_Object *obj, void *event_info)
{
   printf("longpressed: %p\n", event_info);
   Elm_Object_Item* item = event_info;

   Evas* e = evas_object_evas_get(obj);

   Evas_Object* win = evas_object_top_get(e);
   Evas_Object* menu = _create_scene_menu(win, item);
   if (!menu) return;
   ResourceView* rv = data;
   evas_object_data_set(menu, "resourceview", rv);
   /*
   evas_object_data_set(menu, "property", p);
   evas_object_data_set(menu, "entry", obj);
   */
   evas_object_show(menu);

  int x, y;
  evas_pointer_canvas_xy_get(e, &x, &y);

   elm_menu_move(menu, x, y);
}



static void
gl_sel(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   //int depth = elm_genlist_item_expanded_depth_get(glit);
   printf("elected item \n");
}

static void
gl_item_sel(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   //int depth = elm_genlist_item_expanded_depth_get(glit);
   //printf("expanded depth for selected item is %d", depth);

   ResourceView* rv = data;
   Scene* s =  elm_object_item_data_get(glit);
   view_scene_set(rv->view, s);
}




static Elm_Genlist_Item_Class *itc1;

ResourceView*
resource_view_new(Evas_Object* win, View* v)
{
  ResourceView* rv = calloc(1, sizeof rv);
  rv->scenes = eina_hash_pointer_new(NULL);
  rv->view = v;

  Evas_Object* gl;

  gl = elm_genlist_add(win);
  rv->gl = gl;
  //evas_object_smart_callback_add(gl, "selected", _gl_selected, NULL);
  evas_object_smart_callback_add(gl, "clicked,double", _gl_double_clicked, NULL);
  evas_object_smart_callback_add(gl, "longpressed", _gl_longpress, rv);
  evas_object_smart_callback_add(gl, "pressed", _gl_pressed, NULL);
  evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //elm_box_pack_end(bx, gl);
  evas_object_show(gl);

  elm_genlist_homogeneous_set(gl, EINA_TRUE);

  itc1 = elm_genlist_item_class_new();
  itc1->item_style     = "default";
  itc1->func.text_get = gl_scene_text_get;
  //itc1->func.content_get  = gl4_content_get;
  //itc1->func.state_get = gl4_state_get;
  //itc1->func.del       = gl4_del;



  return rv;
}

void
resource_view_scene_add(ResourceView* rv, const Scene* s)
{
  static Elm_Object_Item* parent = NULL;

  Elm_Object_Item* eoi = elm_genlist_item_append(
        rv->gl,
        itc1,
        s,
        NULL,
        ELM_GENLIST_ITEM_NONE,
        gl_item_sel,
        rv);

  property_holder_genlist_item_add(&s->name, eoi);
  printf("I add scene %p \n", eoi);
  eina_hash_add(rv->scenes, &s, eoi);
}

