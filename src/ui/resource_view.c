#include "ui/resource_view.h"
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
  //evas_object_smart_callback_add(gl, "longpressed", _gl_longpress, NULL);
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

  printf("I add scene %p \n", eoi);
  eina_hash_add(rv->scenes, &s, eoi);

}
