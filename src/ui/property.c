#include <Elementary.h>
#include <Eina.h>
#include "property.h"

static void
_entry_changed_cb(void *data, Evas_Object *obj, void *event)
{
  Context *c = data;
  Object *o = context_get_object(c);
  if (o == NULL) return;

  Prop* p = evas_object_data_get(obj, "property");

  switch(p->type) {
    case EET_T_DOUBLE:
       {
        double v =  elm_spinner_value_get(obj);
        memcpy((void*)o + p->offset, &v, sizeof v);
       }
      break;
    default:
      fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
      break;
   }
}

Evas_Object* 
property_add_entry(Evas_Object* win, Evas_Object* bx, char* name, char* value)
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
  
  elm_spinner_step_set(en, 0.1);
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
_property_add_spinner(Property *pw, Prop* p)
{
  Evas_Object *en, *bx2, *label;

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
  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, pw->context);

  evas_object_data_set(en, "property", p);

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
property_update(Property* pw, Eina_List* objects)
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

  if (last != NULL) {
    Prop *p;
    EINA_INARRAY_FOREACH(pw->arr, p) {
      //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
      //printf("   value is : ");
      switch(p->type) {
        case EET_T_DOUBLE:
           {
            double d;
            memcpy(&d, (void*)last + p->offset, sizeof d);
            //printf("%f\n",d);
            elm_spinner_value_set(eina_hash_find(pw->properties, p->name), d );
           }
          break;
        default:
          //printf("novalue \n ");
          break;
      }
    }
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
property_set(Property* pw, Eina_Inarray* a)
{
  pw->arr = a;

  Prop *p;
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
     default:
       printf("novalue \n ");
   }
  }
}

Eina_Inarray* create_property_set()
{
  return eina_inarray_new(sizeof(Prop), 0);
}

#include "component/transform.h"
void my_test(Property* pw)
{
  Eina_Inarray * iarr = create_property_set();

  /*
  ADD_PROP(iarr, Transform, test, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.Z, EET_T_DOUBLE);
  */
  ADD_PROP(iarr, Object, Position.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, Position.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Object, Position.Z, EET_T_DOUBLE);

  property_set(pw, iarr);
}

Property* 
create_property(Evas_Object* win, Context* context)
{
  Property *p = calloc(1, sizeof *p);
  p->context = context;
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

  /*
  Evas_Object *eo;
  eo = _property_add_spinner(p, "x");
  eo = property_add_spinner(p, win, bx, "y");
  eo = property_add_spinner(p, win, bx, "z");

  eo = property_add_spinner(p, win, bx, "yaw");
  eo = property_add_spinner(p, win, bx, "pitch");
  eo = property_add_spinner(p, win, bx, "roll");
  eo = property_add_fileselect(p, win, bx, "mesh");
  */

  my_test(p);

  return p;
}

