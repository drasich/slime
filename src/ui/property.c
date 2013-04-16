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
  Object *o = c->object;
  if (o == NULL) return;

  double v =  elm_spinner_value_get(obj);
  const char *name = evas_object_name_get(obj);

  if (!strcmp(name, "x")) {
    o->Position.X = v;
  } else if (!strcmp(name, "y")) {
    o->Position.Y = v;
  } else if (!strcmp(name, "z")) {
    o->Position.Z = v;
  } else if (!strcmp(name, "yaw")) {
    o->angles.Y = v;
  } else if (!strcmp(name, "pitch")) {
    o->angles.X = v;
  } else if (!strcmp(name, "roll")) {
    o->angles.Z = v;
  }

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

static Evas_Object* 
property_add_spinner(Property *p, Evas_Object* win, Evas_Object* bx, char* name)
{
  Evas_Object *en, *bx2, *label;

  en = elm_spinner_add(win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  //elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  elm_box_pack_end(bx, en);

  evas_object_name_set(en, name);
  
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);
  char s[50];
  sprintf(s, "%s : %s", name, "%f");
  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, name);

  eina_hash_add(
        p->properties,
        name,
        en);
  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, p->context);

  return en;
}

static Evas_Object*
property_add_fileselect(Property *p, Evas_Object* win, Evas_Object* bx, char* name)
{
  Evas_Object *en, *bx2, *label;

  en = elm_spinner_add(win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  //elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  elm_box_pack_end(bx, en);

  evas_object_name_set(en, name);
  
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);
  char s[50];
  sprintf(s, "%s : %s", name, "%f");
  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, name);

  eina_hash_add(
        p->properties,
        name,
        en);
  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, p->context);

  return en;

}


void
property_update(Property* p, Object* o)
{
  Vec3 v = o->Position;

  elm_spinner_value_set(eina_hash_find(p->properties, "x"), v.Y );
  elm_spinner_value_set(eina_hash_find(p->properties, "y"), v.Y );
  elm_spinner_value_set(eina_hash_find(p->properties, "z"), v.Z );
}

static void
_property_entry_free_cb(void *data)
{
   free(data);
}

static Eina_Bool
_property_entry_foreach_cb(
      const Eina_Hash *properties, 
      const void *key,
      void *data,
      void *fdata)
{
  const char *name = key;
  const char *number = data;
  printf("%s: %s\n", name, number);

  // Return EINA_FALSE to stop this callback from being called
  return EINA_TRUE;
}

Property* 
create_property(Evas_Object* win, Context* context)
{
  Property *p = calloc(1, sizeof *p);
  p->context = context;
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

  p->properties = NULL;
  p->properties = eina_hash_string_superfast_new(_property_entry_free_cb);

  Evas_Object *eo;
  eo = property_add_spinner(p, win, bx, "x");
  eo = property_add_spinner(p, win, bx, "y");
  eo = property_add_spinner(p, win, bx, "z");

  eo = property_add_spinner(p, win, bx, "yaw");
  eo = property_add_spinner(p, win, bx, "pitch");
  eo = property_add_spinner(p, win, bx, "roll");
  eo = property_add_fileselect(p, win, bx, "mesh");

  p->root = frame;
  p->box = bx;
  return p;
}


