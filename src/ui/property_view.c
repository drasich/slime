#include <Elementary.h>
#include <Eina.h>
#include "ui/property_view.h"
#include "property.h"

static void
_entry_changed_cb(void *data, Evas_Object *obj, void *event)
{
  //TODO handle when there are many objects

  PropertyView* pw = data;

  Context *c = pw->context;
  Object *o = context_get_object(c);
  if (o == NULL) return;

  Property* p = evas_object_data_get(obj, "property");

  switch(p->type) {
    case EET_T_DOUBLE:
       {
        double v =  elm_spinner_value_get(obj);
        memcpy((void*)o + p->offset, &v, sizeof v);
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

  Control* ct = pw->control;
  control_property_changed(ct, o, p);
}

static Evas_Object* 
_property_add_entry(PropertyView *pw, Property* p)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(pw->win);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(pw->win);
  char s[50];
  sprintf(s, "<b> %s </b> : ", p->name);

  elm_object_text_set(label, s);
  evas_object_show(label);
  elm_box_pack_end(bx2, label);

  en = elm_entry_add(pw->win);
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
        pw->properties,
        p->name,
        en);

  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, pw);
  evas_object_data_set(en, "property", p);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(pw->box, bx2);
  evas_object_show(bx2);

  return en;

}

static Evas_Object* 
_property_add_spinner(PropertyView *pw, Property* p)
{
  Evas_Object *en, *label;

  en = elm_spinner_add(pw->win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  //elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  elm_box_pack_end(pw->box, en);

  evas_object_name_set(en, p->name);
  
  elm_spinner_step_set(en, 0.1);
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);

  char s[50];
  sprintf(s, "%s : %s", p->name, "%f");

  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        pw->properties,
        p->name,
        en);
  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, pw);

  evas_object_data_set(en, "property", p);

  return en;
}


static Evas_Object*
property_add_fileselect(PropertyView *p, Evas_Object* win, Evas_Object* bx, char* name)
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

static void
_property_update_object(PropertyView* pw, Object* o)
{
  Property *p;
  EINA_INARRAY_FOREACH(pw->arr, p) {
    //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
    //printf("   value is : ");
    switch(p->type) {
      case EET_T_DOUBLE:
         {
          double d;
          memcpy(&d, (void*)o + p->offset, sizeof d);
          //printf("%f\n",d);
          elm_spinner_value_set(eina_hash_find(pw->properties, p->name), d );
         }
        break;
      case EET_T_STRING:
         {
          const char** str = (void*)o + p->offset;
          elm_object_text_set(eina_hash_find(pw->properties, p->name), *str );
          printf("how many time you come here\n");
         }
        break;
      default:
        fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
        break;
    }
  }
}

void
property_update(PropertyView* pw, Eina_List* objects)
{
  Eina_List *l;
  Object *o = NULL;
  Object *last = NULL;
  float i = 0;
  Vec3 v = vec3_zero();

  EINA_LIST_FOREACH(objects, l, o) {
    v = vec3_add(v, o->Position);
    last = o;
    i++;
  }

  if (i == 1) {
    _property_update_object(pw, last);
  }
  else {
    //just update the transform

  }

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

void
property_set(PropertyView* pw, Eina_Inarray* a)
{
  pw->arr = a;

  Property *p;
  EINA_INARRAY_FOREACH(a, p) {
   printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
   printf("   value is : ");
   switch(p->type) {
     case EET_T_DOUBLE:
        {
         //double d;
         //memcpy(&d, (void*)&yep + p->offset, sizeof d);
         //printf("%f\n",d);
         _property_add_spinner(pw, p);
        }
         break;
     case EET_T_STRING:
         _property_add_entry(pw, p);
         break;
     default:
         fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
         break;
   }
  }
}

Eina_Inarray* 
object_init_array_properties()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, Object, name, EET_T_STRING);
  ADD_PROP(iarr, Object, Position.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, Position.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, Position.Z, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, angles.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, angles.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, angles.Z, EET_T_DOUBLE);

  return iarr;

}

static Eina_Inarray*
_objects_init_array_properties()
{
  Eina_Inarray* iarr = create_property_set();

  ADD_PROP(iarr, Object, Position.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, Position.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, Position.Z, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, angles.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, angles.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, angles.Z, EET_T_DOUBLE);
  return iarr;

}


PropertyView* 
create_property(Evas_Object* win, Context* context, Control* control)
{
  PropertyView *p = calloc(1, sizeof *p);
  p->context = context;
  p->control = control;
  p->win = win;

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
  p->root = frame;


  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_box_align_set(bx, 0.0, 0.0);
  evas_object_show(bx);
  p->box = bx;

  //elm_object_content_set(frame, bx);
  elm_object_content_set(frame, scroller);
  elm_object_content_set(scroller, bx);

  p->properties = NULL;
  p->properties = eina_hash_string_superfast_new(_property_entry_free_cb);

  Eina_Inarray* arr = object_init_array_properties();
  property_set(p,arr);

  return p;
}

