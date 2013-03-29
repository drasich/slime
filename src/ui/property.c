#include <Elementary.h>
#include <Eina.h>
#include "property.h"

Evas_Object*
property_create(Evas_Object* win)
{
  Evas_Object *frame, *scroller, *bx;

  scroller = elm_scroller_add(win);
  evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(scroller);
  elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
  //elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

  frame = elm_frame_add(win);
  elm_object_text_set(frame, "Properties");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  //elm_box_pack_end(bigbox, frame);
  evas_object_show(frame);


  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_box_align_set(bx, 0.0, 0.0);
  evas_object_show(bx);

  //elm_object_content_set(frame, bx);
  elm_object_content_set(frame, scroller);
  elm_object_content_set(scroller, bx);


  property_add(win, bx, "x", "5");
  property_add(win, bx, "y", "6");
  property_add(win, bx, "z", "7");

  return frame;
}

static void
_entry_changed_cb(void *data, Evas_Object *obj, void *event)
{
  Context *c = data;
  printf("new value %s \n", elm_object_text_get(obj));

  //aid->width = atoi(elm_object_text_get(obj));
}

Evas_Object* 
property_add(Evas_Object* win, Evas_Object* bx, char* name, char* value)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(win);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(win);
  char s[50];
  sprintf(s, "<b> %s </b> : ", name);

  elm_object_text_set(label, s);
  evas_object_show(label);
  elm_box_pack_end(bx2, label);

  en = elm_entry_add(win);
  elm_entry_scrollable_set(en, EINA_TRUE);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  elm_object_text_set(en, value);
  elm_entry_scrollbar_policy_set(en, 
        ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  elm_entry_single_line_set(en, EINA_TRUE);
  //elm_entry_select_all(en);
  evas_object_show(en);
  elm_box_pack_end(bx2, en);

  evas_object_name_set(en, name);
  //evas_object_smart_callback_add(
   //     en, "changed,user", property_changed_cb, operator_);


  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(bx, bx2);
  evas_object_show(bx2);

  return en;

}

Evas_Object* 
property_add_spinner(Evas_Object* win, Evas_Object* bx, char* name, char* value)
{
  Evas_Object *en, *bx2, *label;

  en = elm_spinner_add(win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  elm_box_pack_end(bx, en);

  evas_object_name_set(en, name);
  
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);
  char s[50];
  sprintf(s, "%s : %s", name, "%f");
  elm_spinner_label_format_set(en, s);

  return en;
}


void
property_update(Property* p, Object* o)
{
  Vec3 v = o->Position;
  /*
  char s[50];
  sprintf(s,"%f", v.X);
  elm_object_text_set(p->entry_x, s);
  sprintf(s,"%f", v.Y);
  elm_object_text_set(p->entry_y, s);
  sprintf(s,"%f", v.Z);
  elm_object_text_set(p->entry_z, s);
  */

  elm_spinner_value_set(p->entry_x, v.X );
  elm_spinner_value_set(p->entry_y, v.Y );
  elm_spinner_value_set(p->entry_z, v.Z );
  
}

Property* 
create_property(Evas_Object* win, Context* context)
{
  Property *p = calloc(1, sizeof *p);
  //p->root = property_create(win);

  Evas_Object *frame, *scroller, *bx;

  scroller = elm_scroller_add(win);
  evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(scroller);
  elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
  //elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

  frame = elm_frame_add(win);
  elm_object_text_set(frame, "Properties");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  //elm_box_pack_end(bigbox, frame);
  evas_object_show(frame);


  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_box_align_set(bx, 0.0, 0.0);
  evas_object_show(bx);

  //elm_object_content_set(frame, bx);
  elm_object_content_set(frame, scroller);
  elm_object_content_set(scroller, bx);


  p->entry_x = property_add_spinner(win, bx, "x", "5");
  p->entry_y = property_add_spinner(win, bx, "y", "6");
  p->entry_z = property_add_spinner(win, bx, "z", "7");
  //evas_object_smart_callback_add(p->entry_x, "changed,user", _entry_changed_cb, context);

  p->root = frame;
  p->box = bx;
  return p;
}

