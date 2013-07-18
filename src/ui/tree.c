#include <Elementary.h>
#include <Eina.h>
#include "tree.h"
#include "object.h"
#include "view.h"
#include "ui/property_view.h"
#define __UNUSED__

static Elm_Genlist_Item_Class *itc1;
static Elm_Genlist_Item_Class *itc4;


char *gl4_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
  char buf[256];
  Object* o = (Object*) data;
  snprintf(buf, sizeof(buf), "%s", o->name);
  return strdup(buf);
}

Evas_Object *gl4_content_get(void *data __UNUSED__, Evas_Object *obj, const char *part)
{
   char buf[PATH_MAX];
   if (!strcmp(part, "elm.swallow.icon"))
     {
      return NULL;
        Evas_Object *ic = elm_icon_add(obj);
        snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
        elm_image_file_set(ic, buf, NULL);
        evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        evas_object_show(ic);
        return ic;
     }
   else if (!strcmp(part, "elm.swallow.end"))
     {
        Evas_Object *ck;
        ck = elm_check_add(obj);
        evas_object_propagate_events_set(ck, EINA_FALSE);
        evas_object_show(ck);
        return ck;
     }
   return NULL;
}

Eina_Bool gl4_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

void gl4_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{
   printf("item deleted.\n");
}

static void
gl4_sel(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   //int depth = elm_genlist_item_expanded_depth_get(glit);
   //printf("expanded depth for selected item is %d", depth);

   View* v = data;
   if (v) { 
     Context* context = v->context;
     //context_clean_objects(context);
     context_add_object(context, (Object*) elm_object_item_data_get(glit));
     property_update(v->property, context->objects);
   }
}


static void
gl4_exp(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
  //TODO expand children
  return;
   Elm_Object_Item *glit = event_info;
   Evas_Object *gl = elm_object_item_widget_get(glit);
   int val = (int)(long) elm_object_item_data_get(glit);
   int i = 0;

   val *= 10;
   for (i = 0; i < 10; i++)
     {
        elm_genlist_item_append(gl, itc4,
                                (void *)(long) (val + i)/* item data */,
                                glit/* parent */,
                                ELM_GENLIST_ITEM_TREE, gl4_sel/* func */,
                                NULL/* func data */);
     }
}

static void
gl4_con(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
gl4_exp_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
gl4_con_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}

static void
_tree_effect_enable_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_genlist_tree_effect_enabled_set(data, EINA_TRUE);
}

static void
_tree_effect_disable_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_genlist_tree_effect_enabled_set(data, EINA_FALSE);
}

static void
gl4_unselect(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   View* v = data;
   if (v) { 
     Context* context = v->context;
     context_remove_object(context, (Object*) elm_object_item_data_get(glit));
     property_update(v->property, context->objects);
   }
}





Tree* 
create_widget_tree(Evas_Object* win, struct _View* v)
{
  Tree *t = calloc(1, sizeof *t);
  t->context = v->context;
  t->control = v->control;
  t->view = v;

  Evas_Object *gli, *bx, *rd1, *rd2, *frame;

  frame = elm_frame_add(win);
  t->root = frame;
  elm_object_text_set(frame, "Tree");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  evas_object_show(frame);

  bx = elm_box_add(win);
  t->box = bx;
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, bx);
  evas_object_show(bx);

  elm_object_content_set(frame, bx);

  gli = elm_genlist_add(win);
  t->gl = gli;
  evas_object_size_hint_align_set(gli, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(gli, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(gli);
  elm_genlist_tree_effect_enabled_set(gli, EINA_TRUE);
  elm_genlist_multi_select_set(gli, EINA_TRUE);
  elm_genlist_reorder_mode_set(gli, EINA_TRUE);  

  itc1 = elm_genlist_item_class_new();
  itc1->item_style     = "default";
  itc1->func.text_get = gl4_text_get;
  itc1->func.content_get  = gl4_content_get;
  itc1->func.state_get = gl4_state_get;
  itc1->func.del       = gl4_del;

  //elm_genlist_item_append(gli, itc1,
        //(void *)1/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_NONE, gl4_sel/* func */,
        //NULL/* func data */);
  //elm_genlist_item_append(gli, itc1,
        //(void *)2/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_NONE, gl4_sel/* func */,
        //NULL/* func data */);
  //elm_genlist_item_append(gli, itc1,
        //(void *)3/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_NONE, gl4_sel/* func */,
        //NULL/* func data */);

  elm_genlist_item_class_ref(itc1);
  elm_genlist_item_class_free(itc1);

  evas_object_smart_callback_add(gli, "expand,request", gl4_exp_req, gli);
  evas_object_smart_callback_add(gli, "contract,request", gl4_con_req, gli);
  evas_object_smart_callback_add(gli, "expanded", gl4_exp, gli);
  evas_object_smart_callback_add(gli, "contracted", gl4_con, gli);
  evas_object_smart_callback_add(gli, "unselected", gl4_unselect, v);

  elm_box_pack_end(bx, gli);

  //evas_object_smart_callback_add(rd1, "changed", _tree_effect_enable_cb, gli);
  //evas_object_smart_callback_add(rd2, "changed", _tree_effect_disable_cb, gli);

  return t;
}


void
tree_add_object(Tree* t,  Object* o)
{
  //elm_genlist_item_append(t->gl, itc4,
        //(void *)1/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_TREE, gl4_sel/* func */,
        //NULL/* func data */);

   if (!itc1) itc1 = elm_genlist_item_class_new();
   itc1->item_style = "default";
   itc1->func.text_get = gl4_text_get;
   itc1->func.content_get = gl4_content_get;
   itc1->func.state_get = gl4_state_get;
   itc1->func.del = gl4_del;

   elm_genlist_item_append(t->gl, itc1,
                                  o,//(void *)(long)55/* item data */,
                                  NULL, //elm_genlist_item_parent_get(gli_selected),
                                  ELM_GENLIST_ITEM_NONE,
                                  //NULL/* func */, NULL/* func data */);
                                  gl4_sel, t->view);

  return;

  Elm_Object_Item* item = elm_genlist_item_append(t->gl, itc1,
        //(void *)1/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_TREE, gl17_sel/* func */,
        //o, NULL/* parent */, ELM_GENLIST_ITEM_TREE, gl4_sel/* func */,
        o, NULL/* parent */, ELM_GENLIST_ITEM_TREE, gl4_sel/* func */,
        //NULL/* func data */);
   t->context);



  // if it was a NONE then go into TREE (if you add children)
  //elm_genlist_item_item_class_update (item, itc4);
  //elm_genlist_item_type_set(item, ELM_GENLIST_ITEM_TREE);


}

static Elm_Object_Item*
_tree_get_item(Tree* t, Object* o)
{ 
  Elm_Object_Item* item = elm_genlist_first_item_get(t->gl);
  if (!item) return;

  Object* eo = (Object*) elm_object_item_data_get(item);

  while (eo != o && item) {
    item = elm_genlist_item_next_get(item);
    eo = (Object*) elm_object_item_data_get(item);
  }

  if (o == eo) {
    return item;
  }
  return NULL;
}


void
tree_select_object(Tree* t, Object* o)
{
  Elm_Object_Item* item = _tree_get_item(t, o);

  if (item)
  elm_genlist_item_selected_set(item, EINA_TRUE);

}

void
tree_unselect_all(Tree* t)
{
  Eina_List* items = eina_list_clone( elm_genlist_selected_items_get(t->gl));
  Eina_List* l;
  Elm_Object_Item* i;

  EINA_LIST_FOREACH(items, l, i) {
    elm_genlist_item_selected_set(i, EINA_FALSE);
  }

  eina_list_free(items);
}

void
tree_select_objects(Tree* t, Eina_List* objects)
{
  Eina_List* l;
  Object* o;
  EINA_LIST_FOREACH(objects, l, o) {
    Elm_Object_Item* item = _tree_get_item(t, o);
    if (item)
    elm_genlist_item_selected_set(item, EINA_TRUE);
  }
}


void
tree_update_object(Tree* t, Object* o)
{ 
  Elm_Object_Item* item = _tree_get_item(t, o);

  if (item)
    elm_genlist_item_update(item);
}


void
tree_remove_object(Tree* t,  Object* o)
{
  Elm_Object_Item* item = elm_genlist_first_item_get(t->gl);
  if (!item) return;

  Object* eo = (Object*) elm_object_item_data_get(item);

  while (eo != o && item) {
    item = elm_genlist_item_next_get(item);
    eo = (Object*) elm_object_item_data_get(item);
  }

  if (o == eo) {
    elm_object_item_del(item);
  }

  return;
}

