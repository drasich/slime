#include "glview.h"
#include "view.h"
#define __UNUSED__


Eina_Bool
_anim(void *data)
{
   elm_glview_changed_set(data);
   return EINA_TRUE;
}

void
_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
  printf("del ani\n");
  Ecore_Animator *ani = evas_object_data_get(obj, "ani");
  ecore_animator_del(ani);
}

Evas_Object*
_create_glview(Evas_Object* win)
{
  Evas_Object *glview;

  glview = elm_glview_add(win);
  if (!gl)
  gl = elm_glview_gl_api_get(glview);
  evas_object_size_hint_align_set(glview, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(glview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_glview_mode_set(glview, ELM_GLVIEW_ALPHA | ELM_GLVIEW_DEPTH);
  elm_glview_resize_policy_set(glview, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
  elm_glview_render_policy_set(glview, ELM_GLVIEW_RENDER_POLICY_ON_DEMAND);
  evas_object_show(glview);

  elm_object_focus_set(glview, EINA_TRUE);
  return glview;
}


