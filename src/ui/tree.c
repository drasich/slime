#include <Elementary.h>
#include <Eina.h>
#include "tree.h"
#include "object.h"
#include "view.h"
#define __UNUSED__

static Elm_Genlist_Item_Class *itc1;
static Elm_Genlist_Item_Class *itc4;

static char *gl4_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
  char buf[256];
  Object* o = (Object*) data;
  snprintf(buf, sizeof(buf), "%s", o->name);
  return strdup(buf);
}

Evas_Object *gl4_content_get(void *data __UNUSED__, Evas_Object *obj, const char *part)
{
  return NULL;

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
gl4_sel(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   //int depth = elm_genlist_item_expanded_depth_get(glit);
   //printf("expanded depth for selected item is %d", depth);

   View* v = data;
   if (v) { 
     Context* context = v->context;
     //context_clean_objects(context);
     context_object_add(context, (Object*) elm_object_item_data_get(glit));
   }
}


static void
gl4_exp(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
  //TODO expand children
   Elm_Object_Item *glit = event_info;
   Evas_Object *gl = elm_object_item_widget_get(glit);
   Object* o = elm_object_item_data_get(glit);
   View* v = data;

   Eina_List*l;
   Object* child;
   EINA_LIST_FOREACH(o->children, l, child) {
     if (eina_list_count(child->children) > 0) {

       Elm_Object_Item* eoi = elm_genlist_item_append(
             gl,
             itc4,
             child,
             glit,// parent
             ELM_GENLIST_ITEM_TREE,
             gl4_sel,
             data);

       eina_hash_add(v->tree->objects, &child, eoi);
     }
     else {
       Elm_Object_Item* eoi = elm_genlist_item_append(
             gl,
             itc1,
             child,
             glit,
             ELM_GENLIST_ITEM_NONE,
             gl4_sel,
             data);
       eina_hash_add(v->tree->objects, &child, eoi);

     }

   }
}

static void
gl4_con(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
  View* v = data;
  Elm_Object_Item *glit = event_info;
  Object* o = elm_object_item_data_get(glit);

  Eina_List* l;
  Object* child;
  EINA_LIST_FOREACH(o->children, l, child) {
    eina_hash_del_by_key(v->tree->objects, &child);
  }

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
     context_object_remove(context, (Object*) elm_object_item_data_get(glit));
   }
}



static void
_context_tree_msg_receive(Context* c, void* tree, const char* msg)
{
  if (!strcmp(msg, "clean_objects"))
  tree_unselect_all(tree);
  else if (!strcmp(msg, "add_object"))
  tree_objects_select(tree, c->objects);
  else if (!strcmp(msg, "objects_changed")) {
    tree_objects_select(tree, c->objects);
  }
}

Tree* 
tree_widget_new(Evas_Object* win, struct _View* v)
{
  Tree *t = calloc(1, sizeof *t);
  t->context = v->context;
  t->control = v->control;
  t->view = v;

  context_add_callback(v->context, _context_tree_msg_receive, t);

  Evas_Object *gli, *bx, *rd1, *rd2, *frame;

  /*
  frame = elm_frame_add(win);
  t->root = frame;
  elm_object_text_set(frame, "Tree");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  evas_object_show(frame);
  */

  bx = elm_box_add(win);
  t->box = bx;
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, bx);
  evas_object_show(bx);
  t->root = bx;

  //elm_object_content_set(frame, bx);

  gli = elm_genlist_add(win);
  t->gl = gli;
  evas_object_size_hint_align_set(gli, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(gli, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(gli);
  elm_genlist_tree_effect_enabled_set(gli, EINA_TRUE);
  elm_genlist_multi_select_set(gli, EINA_TRUE);
  elm_genlist_reorder_mode_set(gli, EINA_TRUE);  
  elm_genlist_multi_select_mode_set(gli, ELM_OBJECT_MULTI_SELECT_MODE_WITH_CONTROL);

  itc1 = elm_genlist_item_class_new();
  itc1->item_style     = "default";
  itc1->func.text_get = gl4_text_get;
  itc1->func.content_get  = gl4_content_get;
  itc1->func.state_get = gl4_state_get;
  itc1->func.del       = gl4_del;

  itc4 = elm_genlist_item_class_new();
  itc4->item_style = "tree_effect";
  itc4->func.text_get = gl4_text_get;
  itc4->func.content_get = gl4_content_get;
  itc4->func.state_get = gl4_state_get;
  itc4->func.del = gl4_del;


  elm_genlist_item_class_ref(itc1);
  elm_genlist_item_class_free(itc1);

  evas_object_smart_callback_add(gli, "expand,request", gl4_exp_req, gli);
  evas_object_smart_callback_add(gli, "contract,request", gl4_con_req, gli);
  evas_object_smart_callback_add(gli, "expanded", gl4_exp, v);
  evas_object_smart_callback_add(gli, "contracted", gl4_con, v);
  evas_object_smart_callback_add(gli, "unselected", gl4_unselect, v);

  elm_box_pack_end(bx, gli);

  //evas_object_smart_callback_add(rd1, "changed", _tree_effect_enable_cb, gli);
  //evas_object_smart_callback_add(rd2, "changed", _tree_effect_disable_cb, gli);

  t->objects = eina_hash_pointer_new(NULL);

  return t;
}

static Elm_Object_Item*
_tree_get_item(Tree* t, Object* o)
{ 
  Elm_Object_Item* item = elm_genlist_first_item_get(t->gl);
  if (!item) {
    return NULL;
  }

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
tree_object_add(Tree* t,  Object* o)
{
  static Elm_Object_Item* parent = NULL;
  if (o->parent)
  parent = _tree_get_item(t, o);

  if (eina_list_count(o->children) > 0) {
    Elm_Object_Item* eoi = elm_genlist_item_append(
          t->gl,
          itc4,
          o,
          parent,
          ELM_GENLIST_ITEM_TREE,
          gl4_sel/* func */,
          t->view);

    printf("I add parent %p \n", eoi);
    eina_hash_add(t->objects, &o, eoi);

    return;
  }
  else {

    Elm_Object_Item* eoi = elm_genlist_item_append(
          t->gl,
          itc1,
          o,
          parent,
          ELM_GENLIST_ITEM_NONE,
          gl4_sel,
          t->view);

    printf("I add leaf %p \n", eoi);
    eina_hash_add(t->objects, &o, eoi);
  }

}

void
tree_object_select(Tree* t, Object* o)
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
tree_objects_select(Tree* t, Eina_List* objects)
{
  Eina_List* l;
  Object* o;

  Eina_List* items = elm_genlist_realized_items_get(t->gl);
  Elm_Object_Item* i;

  EINA_LIST_FOREACH(items, l, i) {
    Object* eo = (Object*) elm_object_item_data_get(i);
    if (eina_list_data_find(objects, eo) == eo) {
      elm_genlist_item_selected_set(i, EINA_TRUE);
    }
    else {
      elm_genlist_item_selected_set(i, EINA_FALSE);
    }
  }

  eina_list_free(items);

  EINA_LIST_FOREACH(objects, l, o) {

    Elm_Object_Item* eoi = eina_hash_find(t->objects, &o);
    if (eoi) {
      elm_genlist_item_selected_set(eoi, EINA_TRUE);
    }
    else {
      Eina_List* path = object_parents_path_get(o);
      Eina_List* pl;
      Object* po;
      EINA_LIST_FOREACH(path, pl, po) {
        eoi = eina_hash_find(t->objects, &po);
        if (eoi) {
          elm_genlist_item_expanded_set(eoi, EINA_TRUE);
        }
      }
      eoi = eina_hash_find(t->objects, &o);
      if (eoi) {
        elm_genlist_item_show(eoi, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
        elm_genlist_item_selected_set(eoi, EINA_TRUE);
      }
    }
  }

  /*
  EINA_LIST_FOREACH(objects, l, o) {
    Elm_Object_Item* item = _tree_get_item(t, o);
    if (item) {
      elm_genlist_item_selected_set(item, EINA_TRUE);
    }
  }
  */
}


void
tree_object_update(Tree* t, Object* o)
{ 
  Elm_Object_Item* item = _tree_get_item(t, o);

  //TODO change leaf to tree if you add a child etc
  //or change tree to leaf
  // if it was a NONE then go into TREE (if you add children)
  //elm_genlist_item_item_class_update (item, itc4);
  //elm_genlist_item_type_set(item, ELM_GENLIST_ITEM_TREE);

  if (item)
    elm_genlist_item_update(item);
}


void
tree_object_remove(Tree* t,  Object* o)
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

void tree_scene_set(Tree* t, struct _Scene* s)
{
  elm_genlist_clear(t->gl);
  eina_hash_free(t->objects);
  t->objects = eina_hash_pointer_new(NULL);

  Eina_List* l;
  Object* o;

  EINA_LIST_FOREACH(s->objects, l, o) {
    tree_object_add(t,o);
  }

}

