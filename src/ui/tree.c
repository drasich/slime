#include <Elementary.h>
#include <Eina.h>
#include "tree.h"
#define __UNUSED__

static Elm_Genlist_Item_Class *itc1;
static Elm_Genlist_Item_Class *itc4;


char *gl4_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   char buf[256];
   snprintf(buf, sizeof(buf), "Item mode %i", (int)(long)data);
   return strdup(buf);
}

Evas_Object *gl4_content_get(void *data __UNUSED__, Evas_Object *obj, const char *part)
{
   char buf[PATH_MAX];
   if (!strcmp(part, "elm.swallow.icon"))
     {
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
gl4_sel(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   int depth = elm_genlist_item_expanded_depth_get(glit);
   printf("expanded depth for selected item is %d", depth);
}


static void
gl4_exp(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
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





Tree* 
create_widget_tree(Evas_Object* win, Context* context)
{
  Tree *t = calloc(1, sizeof *t);
  t->context = context;

  Evas_Object *gl, *bx, *bx2, *rd1, *rd2;

  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, bx);
  evas_object_show(bx);

  gl = elm_genlist_add(win);
  evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(gl);
  elm_genlist_tree_effect_enabled_set(gl, EINA_TRUE);

  itc1 = elm_genlist_item_class_new();
  itc1->item_style     = "default";
  itc1->func.text_get = gl4_text_get;
  itc1->func.content_get  = gl4_content_get;
  itc1->func.state_get = gl4_state_get;
  itc1->func.del       = gl4_del;

  itc4 = elm_genlist_item_class_new();
  itc4->item_style     = "tree_effect";
  itc4->func.text_get = gl4_text_get;
  itc4->func.content_get  = gl4_content_get;
  itc4->func.state_get = gl4_state_get;
  itc4->func.del       = gl4_del;

  elm_genlist_item_append(gl, itc4,
        (void *)1/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_TREE, gl4_sel/* func */,
        NULL/* func data */);
  elm_genlist_item_append(gl, itc4,
        (void *)2/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_TREE, gl4_sel/* func */,
        NULL/* func data */);
  elm_genlist_item_append(gl, itc1,
        (void *)3/* item data */, NULL/* parent */, ELM_GENLIST_ITEM_NONE, gl4_sel/* func */,
        NULL/* func data */);

  elm_genlist_item_class_free(itc1);
  elm_genlist_item_class_free(itc4);

  evas_object_smart_callback_add(gl, "expand,request", gl4_exp_req, gl);
  evas_object_smart_callback_add(gl, "contract,request", gl4_con_req, gl);
  evas_object_smart_callback_add(gl, "expanded", gl4_exp, gl);
  evas_object_smart_callback_add(gl, "contracted", gl4_con, gl);

  elm_box_pack_end(bx, gl);

  evas_object_smart_callback_add(rd1, "changed", _tree_effect_enable_cb, gl);
  evas_object_smart_callback_add(rd2, "changed", _tree_effect_disable_cb, gl);

  return t;
}


