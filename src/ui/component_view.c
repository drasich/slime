#include <Elementary.h>
#include <Eina.h>
#include "ui/property_view.h"
#include "property.h"

static void
_entry_changed_cb(void *data, Evas_Object *obj, void *event)
{
  //printf ("todo : at %s, line %d\n",__FILE__, __LINE__);
  //return;
  //PropertyView* pw = data;
  ComponentProperties* cp = data;
  void* o = cp->data;

  if (o == NULL) return;

  Property* p = evas_object_data_get(obj, "property");

  switch(p->type) {
    case EET_T_DOUBLE:
       {
        double v =  elm_spinner_value_get(obj);
        //memcpy((void*)o + p->offset, &v, sizeof v);
        memcpy((void*)o + p->offset, &v, p->size);
       }
      break;
    case EET_T_STRING:
       {
        const char** str = (void*)o + p->offset;
        eina_stringshare_del(*str);
        const char* s = elm_object_text_get(obj);
        *str = eina_stringshare_add(s);
        //eina_stringshare_dump();
       }
      break;
    default:
      fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
      break;
   }

  Control* ct = cp->control;

  if (cp->callback) {
    cp->callback(ct, o, p);
  }

  if (!strcmp(cp->name, "transform"))
  control_property_update(ct, o);
}

static void
_entry_activated_cb(void *data, Evas_Object *obj, void *event)
{
  printf("activated\n");
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(cp->value_saved, s)) {
    Property* p = evas_object_data_get(obj, "property");
    control_change_property(cp->control, cp->data, p, cp->value_saved, s);
  }
}

static void
_entry_aborted_cb(void *data, Evas_Object *obj, void *event)
{
  printf("aborted\n");

  ComponentProperties* cp = data;
  void* o = cp->data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(cp->value_saved, s)) {
    eina_stringshare_del(s);

    Property* p = evas_object_data_get(obj, "property");
    const char** str = (void*)o + p->offset;
    *str = eina_stringshare_add(cp->value_saved);
    elm_object_text_set(obj, *str);

    Control* ct = cp->control;
    control_property_update(ct, o);
  }

}

static void
_entry_focused_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);
  const char* str = eina_stringshare_add(s);
  //TODO don't forget to eina_stringshare_del
  printf("TODO stringshare del\n");
  cp->value_saved = str;
}

static void
_entry_unfocused_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);
  if (strcmp(cp->value_saved, s)) {
    Property* p = evas_object_data_get(obj, "property");
    control_change_property(cp->control, cp->data, p, cp->value_saved, s);
  }
}



static Evas_Object* 
_property_add_entry(ComponentProperties* cp, Property* p)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(cp->win);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(cp->win);
  char s[256];
  sprintf(s, "<b> %s </b> : ", p->name);

  elm_object_text_set(label, s);
  evas_object_show(label);
  elm_box_pack_end(bx2, label);

  en = elm_entry_add(cp->win);
  elm_entry_scrollable_set(en, EINA_TRUE);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  elm_object_text_set(en, "none");
  elm_entry_scrollbar_policy_set(en, 
        ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  elm_entry_single_line_set(en, EINA_TRUE);
  //elm_entry_select_all(en);
  evas_object_show(en);
  elm_box_pack_end(bx2, en);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        cp->properties,
        p->name,
        en);

  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, cp);
  evas_object_smart_callback_add(en, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(en, "aborted", _entry_aborted_cb, cp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, cp);
  evas_object_data_set(en, "property", p);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(cp->box, bx2);
  evas_object_show(bx2);

  return en;

}


static Evas_Object* 
_property_add_spinner(ComponentProperties* cp, Property* p)
{
  Evas_Object *en, *label;

  en = elm_spinner_add(cp->win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  //elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  elm_box_pack_end(cp->box, en);

  evas_object_name_set(en, p->name);
  
  elm_spinner_step_set(en, 0.1);
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);

  char s[50];
  sprintf(s, "%s : %s", p->name, "%f");

  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        cp->properties,
        p->name,
        en);

  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, cp);

  evas_object_data_set(en, "property", p);

  return en;
}


/*
static Evas_Object*
property_add_fileselect(PropertyView *pw, Evas_Object* win, Evas_Object* bx, char* name)
{
//TODO
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
        pw->properties,
        name,
        en);
  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, p->context);

  return en;
}
  */

void
component_property_update_data(ComponentProperties* cp, void* data)
{
  Property *p;

  EINA_INARRAY_FOREACH(cp->arr, p) {
    Evas_Object* obj = eina_hash_find(cp->properties, p->name);
    //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
    //printf("   value is : ");
    switch(p->type) {
      case EET_T_DOUBLE:
         {
          double d;
          memcpy(&d, (void*)data + p->offset, sizeof d);
          //printf("%f\n",d);
          double old = elm_spinner_value_get(obj);
          if (old != d) {
          printf("value is different\n");
          elm_spinner_value_set(eina_hash_find(cp->properties, p->name), d );
          }
         }
        break;
      case EET_T_STRING:
         {
          const char** str = (void*)data + p->offset;
          const char* s = elm_object_text_get(obj);
          if (strcmp(*str,s)) 
            elm_object_text_set(obj, *str );
         }
        break;
      default:
        fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
        break;
    }
  }
}

static void
_property_entry_free_cb(void *data)
{
   //free(data);
}


ComponentProperties*
create_my_prop(const char* name, Eina_Inarray *a, Evas_Object* win, Control* control)
{
  ComponentProperties* cp = calloc(1, sizeof *cp);
  cp->arr = a;
  cp->win = win;
  cp->control = control;
  cp->properties = eina_hash_string_superfast_new(_property_entry_free_cb);
  cp->name = name;

  Evas_Object* frame = elm_frame_add(win);
  char s[256];
  sprintf(s, "Component %s", name);
  elm_object_text_set(frame, s);
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  //evas_object_show(frame);

  cp->box = elm_box_add(win);
  evas_object_size_hint_weight_set(cp->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(cp->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_align_set(cp->box, 0.0, 0.0);
  //evas_object_show(cp->box);
  elm_object_content_set(frame, cp->box);
  cp->root = frame;
  
  /*
  Evas_Object* label = elm_label_add(cp->win);
  char s[256];
  sprintf(s, "Component <b>%s</b>", name);

  elm_object_text_set(label, s);
  evas_object_show(label);
  elm_box_pack_end(cp->box, label);
  */

  Property *p;
  EINA_INARRAY_FOREACH(a, p) {
   //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
   //printf("   value is : ");
   switch(p->type) {
     case EET_T_DOUBLE:
        {
         //double d;
         //memcpy(&d, (void*)&yep + p->offset, sizeof d);
         //printf("%f\n",d);
         _property_add_spinner(cp, p);
        }
         break;
     case EET_T_STRING:
         _property_add_entry(cp, p);
         break;
     default:
         fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
         break;
   }
  }
  return cp;
}

