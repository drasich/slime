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
  MyProp* mp = data;
  void* o = mp->data;

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

  Control* ct = mp->control;

  if (mp->callback) {
    mp->callback(ct, o, p);
  }

  control_property_update(ct, o);
}

static void
_entry_activated_cb(void *data, Evas_Object *obj, void *event)
{
  printf("activated\n");
  MyProp* mp = data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(mp->value_saved, s)) {
    Property* p = evas_object_data_get(obj, "property");
    control_change_property(mp->control, mp->data, p, mp->value_saved, s);
  }
}

static void
_entry_aborted_cb(void *data, Evas_Object *obj, void *event)
{
  printf("aborted\n");

  MyProp* mp = data;
  void* o = mp->data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(mp->value_saved, s)) {
    eina_stringshare_del(s);

    Property* p = evas_object_data_get(obj, "property");
    const char** str = (void*)o + p->offset;
    *str = eina_stringshare_add(mp->value_saved);
    elm_object_text_set(obj, *str);

    Control* ct = mp->control;
    control_property_update(ct, o);
  }

}

static void
_entry_focused_cb(void *data, Evas_Object *obj, void *event)
{
  MyProp* mp = data;
  const char* s = elm_object_text_get(obj);
  const char* str = eina_stringshare_add(s);
  //TODO don't forget to eina_stringshare_del
  printf("TODO stringshare del\n");
  mp->value_saved = str;
}

static void
_entry_unfocused_cb(void *data, Evas_Object *obj, void *event)
{
  MyProp* mp = data;
  const char* s = elm_object_text_get(obj);
  if (strcmp(mp->value_saved, s)) {
    Property* p = evas_object_data_get(obj, "property");
    control_change_property(mp->control, mp->data, p, mp->value_saved, s);
  }
}



static Evas_Object* 
_property_add_entry(MyProp* mp, Property* p)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(mp->win);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(mp->win);
  char s[256];
  sprintf(s, "<b> %s </b> : ", p->name);

  elm_object_text_set(label, s);
  evas_object_show(label);
  elm_box_pack_end(bx2, label);

  en = elm_entry_add(mp->win);
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
        mp->properties,
        p->name,
        en);

  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, mp);
  evas_object_smart_callback_add(en, "activated", _entry_activated_cb, mp);
  evas_object_smart_callback_add(en, "aborted", _entry_aborted_cb, mp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, mp);
  evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, mp);
  evas_object_data_set(en, "property", p);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(mp->box, bx2);
  evas_object_show(bx2);

  return en;

}


static Evas_Object* 
_property_add_spinner(MyProp* mp, Property* p)
{
  Evas_Object *en, *label;

  en = elm_spinner_add(mp->win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  //elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  elm_box_pack_end(mp->box, en);

  evas_object_name_set(en, p->name);
  
  elm_spinner_step_set(en, 0.1);
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);

  char s[50];
  sprintf(s, "%s : %s", p->name, "%f");

  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        mp->properties,
        p->name,
        en);

  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, mp);

  evas_object_data_set(en, "property", p);

  return en;
}


static Evas_Object*
property_add_fileselect(PropertyView *pw, Evas_Object* win, Evas_Object* bx, char* name)
{
//TODO
/*
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
  */
}

void
_property_update_data(MyProp* mp, void* data)
{
  Property *p;

  EINA_INARRAY_FOREACH(mp->arr, p) {
    Evas_Object* obj = eina_hash_find(mp->properties, p->name);
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
          elm_spinner_value_set(eina_hash_find(mp->properties, p->name), d );
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

void property_clear_components(PropertyView* pw)
{
  elm_box_unpack_all(pw->box);

  Eina_List* l;
  MyProp* mp;

  EINA_LIST_FOREACH(pw->component_widgets, l, mp) {
    evas_object_hide(mp->box);
  }

  pw->component_widgets = eina_list_free(pw->component_widgets);
}

void
property_add_component(PropertyView* pw, MyProp* mp)
{
  elm_box_pack_end(pw->box, mp->box);
  evas_object_show(mp->box);
  pw->component_widgets = eina_list_append(pw->component_widgets, mp);
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

  int nb = eina_list_count(objects);
  if (nb == 1) {
    MyProp* mp = eina_hash_find(pw->component_widgets_backup,"transform");
    //TODO clean and add
    property_clear_components(pw);
    //property_set(pw, mp);
    property_add_component(pw, mp);
    mp->data = last;
    pw->current = mp;
    //_property_update_data(pw->current, last);
    _property_update_data(mp, last);

    //TODO add the components widget

  }
  else if (nb > 1) {
    property_set(pw, NULL);
    //MyProp* mp = eina_hash_find(pw->component_widgets_backup,"multiple");
    //mp->data = &pw->context->mos;
    //_property_update_data(pw->current, &pw->context->mos);
  }
}

//TODO change this function to something like update_component_transform 
//and remove current
void
property_update2(PropertyView* pw, Object* o)
{
  if ( pw->current->data == o) {
    _property_update_data(pw->current, o);
  }
}


static void
_property_entry_free_cb(void *data)
{
   //free(data);
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
property_clean(PropertyView* pw)
{
  elm_box_clear(pw->box);
}

void
property_set(PropertyView* pw, MyProp* mp)
{
  if (pw->current) {
    //elm_object_content_unset(pw->scroller);
    evas_object_hide(pw->current->box);
    elm_box_unpack_all(pw->box);
  }

  if (mp) {
    //elm_object_content_set(pw->scroller, mp->box);
    //evas_object_show(mp->box);
    elm_box_pack_end(pw->box, mp->box);
    evas_object_show(mp->box);
  }

  pw->current = mp;
}

static Eina_Inarray* 
_object_init_array_properties()
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
_multiple_objects_init_array_properties()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, Multiple_Objects_Selection, center.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Multiple_Objects_Selection, center.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Multiple_Objects_Selection, center.Z, EET_T_DOUBLE);
  ADD_PROP(iarr, Multiple_Objects_Selection, rotation.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Multiple_Objects_Selection, rotation.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Multiple_Objects_Selection, rotation.Z, EET_T_DOUBLE);

  return iarr;
}

MyProp*
create_my_prop(Eina_Inarray *a, Evas_Object* win, Control* control)
{
  MyProp* mp = calloc(1, sizeof *mp);
  mp->arr = a;
  mp->win = win;
  mp->control = control;
  mp->properties = eina_hash_string_superfast_new(_property_entry_free_cb);

  mp->box = elm_box_add(win);
  evas_object_size_hint_weight_set(mp->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(mp->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_align_set(mp->box, 0.0, 0.0);
  //evas_object_show(mp->box);

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
         _property_add_spinner(mp, p);
        }
         break;
     case EET_T_STRING:
         _property_add_entry(mp, p);
         break;
     default:
         fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
         break;
   }
  }
  return mp;
}

static void
_changed_multiple_object(Control* c, void* data, Property* p)
{
  printf("changed multiple object prop\n");
  
  Multiple_Objects_Selection* mos = data;

}

static void
_context_property_msg_receive(Context* c, void* propertyview, const char* msg)
{
  printf("context msg received : %s\n", msg);
  property_update(propertyview, c->objects);
}


PropertyView* 
create_property(Evas_Object* win, Context* context, Control* control)
{
  PropertyView *p = calloc(1, sizeof *p);
  p->context = context;
  p->control = control;
  p->win = win;

  context_add_callback(context, _context_property_msg_receive, p);

  Evas_Object *frame, *scroller, *bx;

  scroller = elm_scroller_add(win);
  evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(scroller);
  elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
  p->scroller = scroller;

  //elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //elm_box_align_set(bx, 0.0, 0.0);
  p->box = bx;
  elm_object_content_set(p->scroller, p->box);
  evas_object_show(bx);

  frame = elm_frame_add(win);
  elm_object_text_set(frame, "Properties");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  evas_object_show(frame);
  p->root = frame;

  elm_object_content_set(frame, scroller);

  p->component_widgets_backup = eina_hash_string_superfast_new(_property_entry_free_cb);

  //TODO remove these arrays, and the myprop from propertyview
  p->arr = _object_init_array_properties();
  p->array_multiple_objects = _multiple_objects_init_array_properties();
  MyProp* transform = create_my_prop(p->arr, win, control);

  eina_hash_add(
        p->component_widgets_backup,
        "transform",
        transform);
  
  MyProp* manyobj = create_my_prop(p->array_multiple_objects, win, control);
  manyobj->callback = _changed_multiple_object;

  eina_hash_add(
        p->component_widgets_backup,
        "multiple",
        transform);

  return p;
}

