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

static char*
gl_prefab_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
  char buf[256];
  Prefab* p = data;
  snprintf(buf, sizeof(buf), "%s", p->prefab->name);
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

  Elm_Object_Item* another = elm_genlist_item_prev_get(item);
  if (!another || (another && elm_genlist_item_type_get(another) != ELM_GENLIST_ITEM_NONE))
    another = elm_genlist_item_next_get(item);

  if (!another || (another && elm_genlist_item_type_get(another) != ELM_GENLIST_ITEM_NONE)) {
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

  if (eina_hash_find(resource_scenes_get(s_rm), "empty")) {
    int index = 0;
    char str[256] = "empty";
    while (eina_hash_find(resource_scenes_get(s_rm), str)) {
      sprintf(str, "empty%d", index);
      index++;
    }
    s->name = eina_stringshare_add(str);

  }

  resource_scene_add(s_rm, s);
  resource_view_scene_add(rv, s);
  view_scene_set(rv->view, s);
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

static void
gl_item_prefab_sel(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   //int depth = elm_genlist_item_expanded_depth_get(glit);
   //printf("expanded depth for selected item is %d", depth);

   ResourceView* rv = data;
   Prefab* p =  elm_object_item_data_get(glit);
   //view_scene_set(rv->view, s);
   property_prefab_show(rv->view->property, p);
}


static char *gl_group_text_get(
      void *data,
      Evas_Object *obj EINA_UNUSED,
      const char *part EINA_UNUSED)
{
  ResourceGroup* rg = data;
  return strdup(rg->name);
}

static void
_context_scene_list_msg_receive(Context* c, void* scene_list, const char* msg)
{
  if (!strcmp(msg, "scene_changed")) {
    resource_view_scene_select(scene_list, c->scene);
  }
}

static void
_resource_view_groups_add(ResourceView* rv)
{
  if (rv->resource_type == RESOURCE_SCENE) {
    rv->group = resource_view_group_add(rv, "Scenes");
    rv->scene_group_playing = resource_view_group_add(rv, "Scenes(Playing)");
  }
  else if (rv->resource_type == RESOURCE_PREFAB) {
    rv->group = resource_view_group_add(rv, "Prefab");
  }
}

static Elm_Genlist_Item_Class *class_scene = NULL;
static Elm_Genlist_Item_Class *class_group = NULL;
static Elm_Genlist_Item_Class *class_prefab = NULL;

ResourceView*
resource_view_new(Evas_Object* win, View* v, ResourceType type)
{
  ResourceView* rv = calloc(1, sizeof *rv);
  //rv->scenes = eina_hash_pointer_new(_scene_free_cb);
  rv->view = v;
  if (type == RESOURCE_SCENE){
    rv->view->rv = rv;
  }
  else if (type == RESOURCE_PREFAB) {
    rv->view->rv_prefab = rv;
  }
  rv->resource_type = type;

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

  if (!class_scene) {
    class_scene = elm_genlist_item_class_new();
    class_scene->item_style     = "default";
    class_scene->func.text_get = gl_scene_text_get;
    //class_scene->func.content_get  = gl4_content_get;
    //class_scene->func.state_get = gl4_state_get;
    //class_scene->func.del       = gl4_del;
  }

  if (!class_prefab) {
    class_prefab = elm_genlist_item_class_new();
    class_prefab->item_style     = "default";
    class_prefab->func.text_get = gl_prefab_text_get;
  }

  if (!class_group) {
    class_group = elm_genlist_item_class_new();
    class_group->item_style = "group_index";
    class_group->func.text_get = gl_group_text_get;
  }

  _resource_view_groups_add(rv);
  if (type == RESOURCE_SCENE)
  context_add_callback(v->context, _context_scene_list_msg_receive, rv);

  elm_genlist_select_mode_set(gl, ELM_OBJECT_SELECT_MODE_ALWAYS);

  return rv;
}

Elm_Object_Item* 
resource_view_group_add(ResourceView* rv, const char* name)
{
  ResourceGroup* rg = calloc(1, sizeof *rg);
  rg->resources = eina_hash_stringshared_new(NULL);
  rg->name = eina_stringshare_add(name);

  Elm_Object_Item* gli = elm_genlist_item_append(
        rv->gl,
        class_group,
        rg,//(void *)(uintptr_t)i/* item data */,
        NULL/* parent */,
        ELM_GENLIST_ITEM_GROUP,
        NULL,//gl_sel/* func */,
        NULL//(void *)(uintptr_t)(i * 10)/* func data */
        );

  elm_genlist_item_select_mode_set(gli, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
  rg->item = gli;

  return gli;
}

void
resource_view_scene_add(ResourceView* rv, const Scene* s)
{
  Elm_Object_Item* eoi = elm_genlist_item_append(
        rv->gl,
        class_scene,
        s,
        rv->group,//NULL,
        ELM_GENLIST_ITEM_NONE,
        gl_item_sel,
        rv);

  property_holder_genlist_item_add(&s->name, eoi);
}

void
resource_view_clean(ResourceView* rv)
{
  elm_genlist_clear(rv->gl);
  _resource_view_groups_add(rv);

  /*
  Elm_Object_Item* item = elm_genlist_first_item_get(rv->gl);

  while (item) {
    Elm_Object* nextitem = elm_genlist_item_next_get(item);
    ss = elm_object_item_data_get(item);
    property_holder_genlist_item_add(&s->name, eoi);
  }

  if (s == ss) {
    elm_object_item_del(item);
  }
  */

}

void
resource_view_playing_scene_add(ResourceView* rv, const Scene* s)
{
  Elm_Object_Item* eoi = elm_genlist_item_append(
        rv->gl,
        class_scene,
        s,
        rv->scene_group_playing,//NULL,
        ELM_GENLIST_ITEM_NONE,
        gl_item_sel,
        rv);

  property_holder_genlist_item_add(&s->name, eoi);
}


void
resource_view_scene_del(ResourceView* rv, const Scene* s)
{
  static Elm_Object_Item* parent = NULL;

  Elm_Object_Item* item = elm_genlist_first_item_get(rv->gl);
  if (!item) return;

  Scene* ss = elm_object_item_data_get(item);

  while (ss != s && item) {
    item = elm_genlist_item_next_get(item);
    ss = elm_object_item_data_get(item);
  }

  if (s == ss) {
    elm_object_item_del(item);
  }

  return;
}

static Elm_Object_Item*
_scene_list_get_item(ResourceView* rv, const Scene* s)
{ 
  Elm_Object_Item* item = elm_genlist_first_item_get(rv->gl);
  if (!item) {
    return NULL;
  }

  Scene* es = elm_object_item_data_get(item);

  while (es != s && item) {
    item = elm_genlist_item_next_get(item);
    es = elm_object_item_data_get(item);
  }

  if (s == es) {
    return item;
  }
  return NULL;
}


void
resource_view_scene_select(ResourceView* rv, const Scene* s)
{
  Elm_Object_Item* item = _scene_list_get_item(rv, s);

  if (item)
  elm_genlist_item_selected_set(item, EINA_TRUE);
}

void
resource_view_update(ResourceView* rv)
{
  resource_view_clean(rv);

  Eina_Hash* hash = NULL;

  if (rv->resource_type == RESOURCE_SCENE)
  hash = resource_scenes_get(s_rm);
  else if (rv->resource_type == RESOURCE_PREFAB)
  hash = resource_prefabs_get(s_rm);


  if (hash) {
    Eina_Iterator* it = eina_hash_iterator_tuple_new(hash);
    void *data;

    while (eina_iterator_next(it, &data)) {
      Eina_Hash_Tuple *t = data;
      //const char* name = t->key;
      if (rv->resource_type == RESOURCE_SCENE) {
        const Scene* s = t->data;
        //printf("key, scene name : %s, %s\n", name, s->name);
        //elm_menu_item_add(menu, NULL, NULL, name, _change_scene, name);
        resource_view_scene_add(rv, s);
      }
      else if (rv->resource_type == RESOURCE_PREFAB) {
        const Prefab* p = t->data;
        //printf("key, scene name : %s, %s\n", name, s->name);
        //elm_menu_item_add(menu, NULL, NULL, name, _change_scene, name);
        resource_view_prefab_add(rv, p);
      }
    }
    eina_iterator_free(it);
  }

}

void
resource_view_prefab_add(ResourceView* rv, const Prefab* p)
{
  Elm_Object_Item* eoi = elm_genlist_item_append(
        rv->gl,
        class_prefab,
        p,
        rv->group,
        ELM_GENLIST_ITEM_NONE,
        gl_item_prefab_sel,
        rv);

  property_holder_genlist_item_add(&p->prefab->name, eoi);
}

