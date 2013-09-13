#include <Elementary.h>
#include <Eina.h>
#include "ui/property_view.h"
#include "property.h"
#include "view.h"

static void
_entry_changed_cb(void *data, Evas_Object *obj, void *event)
{
  //printf ("todo : at %s, line %d\n",__FILE__, __LINE__);
  //return;
  //PropertyView* pw = data;
  ComponentProperties* cp = data;
  void* cd = cp->component->data;

  if (cd == NULL) return;

  Property* p = evas_object_data_get(obj, "property");

  switch(p->type) {
    case EET_T_DOUBLE:
       {
        double v =  elm_spinner_value_get(obj);
        //memcpy((void*)cd + p->offset, &v, sizeof v);
        //memcpy((void*)cd + p->offset, &v, p->size);
        memcpy(cd + p->offset, &v, p->size);
       }
      break;
    case EET_T_STRING:
       {
        const char** str = (void*)cd + p->offset;
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
    cp->callback(ct, cd, p);
  }

  if (cp->component && cp->component->funcs->on_property_changed)
  cp->component->funcs->on_property_changed(cp->component);

  if (!strcmp(cp->name, "object"))
  control_property_update(ct, cp->component);
}

static void
_entry_activated_cb(void *data, Evas_Object *obj, void *event)
{
  printf("activated\n");
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(cp->value_saved, s)) {
    Property* p = evas_object_data_get(obj, "property");

    if (p->type == EET_T_STRING)
    control_change_property(cp->control, cp->component, p, cp->value_saved, s);
    else if (p->type == PROPERTY_POINTER) {
      //TODO c'est un peu naze de get la scene comme ca
      Scene* scene = cp->control->view->context->scene;
      Object* o = scene_object_get(scene, s);
      if (o) printf("object ok \n");
      else
      printf("object NULL \n");
      Object* old = scene_object_get(scene, cp->value_saved);
      if (old)
      printf("old name :%s \n", old->name);
      else
      printf("old is null\n");

      Object* pointer = component_property_data_get(cp->component, p);
      if ( old == pointer)
      printf("old == pointer %p, %p \n", old, pointer);
      else 
      printf("old != pointer %p, %p \n", old, pointer);
      if (pointer)
      printf("pointer name :%s \n", (pointer)->name);
      printf("o name :%s \n", o->name);
      control_change_property(cp->control, cp->component, p, pointer, o);

    }

    cp->value_saved = s;
  }

}

static void
_entry_aborted_cb(void *data, Evas_Object *obj, void *event)
{
  printf("aborted\n");

  ComponentProperties* cp = data;
  Component* component = cp->component;
  void* cd = component->data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(cp->value_saved, s)) {
    eina_stringshare_del(s);

    Property* p = evas_object_data_get(obj, "property");
    const char** str = (void*)cd + p->offset;
    *str = eina_stringshare_add(cp->value_saved);
    elm_object_text_set(obj, *str);

    Control* ct = cp->control;
    control_property_update(ct, component);
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
    cp->value_saved = s;
    Property* p = evas_object_data_get(obj, "property");
    control_change_property(cp->control, cp->component, p, cp->value_saved, s);
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
  //elm_entry_scrollbar_policy_set(en, 
  //      ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  elm_entry_single_line_set(en, EINA_TRUE);
  //elm_entry_select_all(en);
  evas_object_show(en);
  elm_box_pack_end(bx2, en);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
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
_property_add_spinner(ComponentProperties* cp, Property* p, Evas_Object* box)
{
  Evas_Object *en, *label;

  en = elm_spinner_add(cp->win);
  evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
  //elm_spinner_value_set(en, atof(value));
  evas_object_show(en);
  //elm_box_pack_end(cp->box, en);
  elm_box_pack_end(box, en);

  evas_object_name_set(en, p->name);
  
  elm_spinner_step_set(en, 0.1);
  elm_spinner_min_max_set(en, -DBL_MAX, DBL_MAX);
  elm_object_style_set (en, "vertical");



  char s[50];
  sprintf(s, "%s : %s", p->name, "%f");

  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
        en);

  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, cp);

  evas_object_data_set(en, "property", p);

  return en;
}

static void
_file_chosen(void *data, Evas_Object *obj, void *event_info)
{
  const char *file = event_info;
  Component* c = data;
  //printf("component name : %s\n", c->name);
  MeshComponent* m = c->data;
  if (file)
   {
    //mesh_file_set(m, file);
    //mesh_resend(m);
   }
}


static Evas_Object*
_property_add_fileselect(ComponentProperties* cp, Property* p)
{
  printf("file select %s \n", cp->name);
  printf("file select %s \n", cp->component->name);
  //TODO
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
  //elm_entry_scrollbar_policy_set(en, 
   //     ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  elm_entry_single_line_set(en, EINA_TRUE);
  elm_entry_editable_set(en, EINA_FALSE);
  //elm_entry_select_all(en);
  evas_object_show(en);
  elm_box_pack_end(bx2, en);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
        en);

  /*
  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, cp);
  evas_object_smart_callback_add(en, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(en, "aborted", _entry_aborted_cb, cp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, cp);
  */
  evas_object_data_set(en, "property", p);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);

  Evas_Object* fs_bt = elm_fileselector_button_add(cp->win);
  elm_object_focus_allow_set(fs_bt, 0);
  elm_fileselector_button_path_set(fs_bt, "/home/chris/code/slime/model");
  elm_object_text_set(fs_bt, "Change");
  //elm_object_part_content_set(fs_bt, "icon", ic);
  elm_fileselector_button_expandable_set(fs_bt, EINA_TRUE);
  //elm_fileselector_mode_set(fs_bt, ELM_FILESELECTOR_LIST);
  elm_fileselector_button_inwin_mode_set(fs_bt, EINA_TRUE);
  evas_object_smart_callback_add(fs_bt, "file,chosen", _file_chosen, cp->component);
  evas_object_show(fs_bt);
  elm_box_pack_end(bx2, fs_bt);
  
  elm_box_pack_end(cp->box, bx2);
  evas_object_show(bx2);

  return en;
}


static void
_component_property_update_data_recur(ComponentProperties* cp, void* data, PropertySet* ps)
{
  Property *p;
  EINA_INARRAY_FOREACH(ps->array, p) {
    Evas_Object* obj = eina_hash_find(cp->properties, &p);
    //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
    switch(p->type) {
      case EET_T_DOUBLE:
         {
          double d;
          memcpy(&d, (void*)data + p->offset, sizeof d);
          //printf("my value is : %f\n");
          //printf("%f\n",d);
          double old = elm_spinner_value_get(obj);
          if (old != d) {
            elm_spinner_value_set(obj, d );
          }
         }
        break;
      case PROPERTY_FILENAME:
      case EET_T_STRING:
         {
          const char** str = (void*)data + p->offset;
          const char* s = elm_object_text_get(obj);
          if (!*str) break;
          if (strcmp(*str,s)) 
            elm_object_text_set(obj, *str );
         }
        break;
     case PROPERTY_STRUCT:
         _component_property_update_data_recur(cp, data, p->array);
         break;
     case PROPERTY_POINTER:
          {
          const void** ptr = (void*)data + p->offset;
          const char* s = elm_object_text_get(obj);
          const Object* o = *ptr;

          if (!o) break;
          if (strcmp(o->name,s)) 
            elm_object_text_set(obj, o->name );

          }

         break;
      default:
        fprintf (stderr, "type not yet implemented: %d at %s, line %d\n",p->type, __FILE__, __LINE__);
        printf("component name: %s, prop name %s \n", cp->name, p->name);
        break;
    }
  }

}

void
component_property_update_data(ComponentProperties* cp, void* data)
{
  _component_property_update_data_recur(cp, data, cp->arr);
}

static void
_property_entry_free_cb(void *data)
{
   //free(data);
}

static void
_remove_component(
      void *data,
      Evas_Object *obj,
      void *event_info)
{
  //TODO remove components and components view
  // how to do when there is the same components many time...
  printf("TODO remove component\n");
  ComponentProperties* cp = data;
  if (cp->pw->context->object)
  control_object_remove_component(cp->control, cp->pw->context->object, cp->component);
}


static void
_add_properties(ComponentProperties* cp, PropertySet* ps, Evas_Object* box)
{
  Property *p;
  EINA_INARRAY_FOREACH(ps->array, p) {
   //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
   //printf("   value is : ");
   switch(p->type) {
     case EET_T_DOUBLE:
        {
         _property_add_spinner(cp, p, box);
        }
         break;
     case EET_T_STRING:
         _property_add_entry(cp, p);
         break;
     case PROPERTY_FILENAME:
         _property_add_fileselect(cp, p);
         break;
     case PROPERTY_STRUCT:
         if (p->array->hint == HORIZONTAL) {
           Evas_Object* hbox = elm_box_add(cp->win);
           elm_box_horizontal_set(hbox, EINA_TRUE);
           evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.0);
           evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
           elm_box_pack_end(box, hbox);
           evas_object_show(hbox);
           //TODO clean box
           //
           Evas_Object* label = elm_label_add(cp->win);
           char s[256];
           sprintf(s, "<b> %s </b> : ", p->name);

           elm_object_text_set(label, s);
           evas_object_show(label);
           elm_box_pack_end(hbox, label);

           _add_properties(cp, p->array, hbox);
         }
         else
         _add_properties(cp, p->array, cp->box);
         break;
     case PROPERTY_POINTER:
         _property_add_entry(cp, p);
         break;
     default:
         fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
         break;
   }
  }

}

ComponentProperties*
create_my_prop(Component* c, Evas_Object* win, Control* control, bool can_remove)
{
  ComponentProperties* cp = calloc(1, sizeof *cp);
  cp->component = c;
  cp->arr = c->properties;
  cp->win = win;
  cp->control = control;
  //cp->properties = eina_hash_string_superfast_new(_property_entry_free_cb);
  cp->properties = eina_hash_pointer_new(_property_entry_free_cb);
  cp->name = c->name;

  Evas_Object* frame = elm_frame_add(win);
  char s[256];
  sprintf(s, "Component %s", c->name);
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

  if (can_remove) {
  Evas_Object* bt = elm_button_add(win);
  elm_object_text_set(bt, "remove");
  evas_object_show(bt);
  elm_box_pack_end(cp->box, bt);
  evas_object_smart_callback_add(bt, "clicked", _remove_component, cp);
  }

  
  /*
  Evas_Object* label = elm_label_add(cp->win);
  char s[256];
  sprintf(s, "Component <b>%s</b>", name);

  elm_object_text_set(label, s);
  evas_object_show(label);
  elm_box_pack_end(cp->box, label);
  */

  _add_properties(cp, c->properties, cp->box);

  return cp;
}

ComponentProperties*
create_component_properties(Component* c, PropertyView* pw)
{
  ComponentProperties* cp = create_my_prop(c, pw->win, pw->control, true);
  //cp->component = c;
  cp->pw = pw;
  return cp;
}

