#include <Elementary.h>
#include <Eina.h>
#include "ui/property_view.h"
#include "property.h"
#include "view.h"
#include "resource.h"

static void _add_properties(
      ComponentProperties* cp,
      const Property* ps,
      Evas_Object* box,
      void* data);

static void
_entry_orientation_changed_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  void* cd = cp->component->data;

  if (cd == NULL) return;

  Property* p = evas_object_data_get(obj, "property");
  const char* name = evas_object_data_get(obj, "property_name");

  double v =  elm_spinner_value_get(obj);
  double saved;
  eina_value_get(&cp->saved, &saved);
  saved = v - saved;
  Quat q = quat_identity();
  if (!strcmp(name, "x")) {
    q = quat_angles_deg(vec3(saved,0,0));
  }
  else if (!strcmp(name, "y")) {
    q = quat_angles_deg(vec3(0,saved,0));
  }
  else if (!strcmp(name, "z")) {
    q = quat_angles_deg(vec3(0,0,saved));
  }

  q = quat_mul(cp->quat_saved, q);
  int offset = property_offset_get(p);
  memcpy(cd + offset, &q, sizeof q);
}

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
        int offset = property_offset_get(p);
        memcpy(cd + offset, &v, p->size);
       }
      break;
    case EET_T_STRING:
       {
        int offset = property_offset_get(p);
        const char** str = (void*)cd + offset;
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
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);

  if (strcmp(cp->value_saved, s)) {
    Property* p = evas_object_data_get(obj, "property");

    if (p->type == EET_T_STRING)
    control_property_change(cp->control, cp->component, p, cp->value_saved, s);
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
      control_property_change(cp->control, cp->component, p, pointer, o);

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
    int offset = property_offset_get(p);
    const char** str = (void*)cd + offset;
    *str = eina_stringshare_add(cp->value_saved);
    elm_object_text_set(obj, *str);

    Control* ct = cp->control;
    control_property_update(ct, component);
  }

}

static void
_change_resource(void *data,
      Evas_Object *obj,
      void *event_info)
{
  Property* p = evas_object_data_get(obj, "property");
  Component* c = evas_object_data_get(obj, "component");
  const char *name = data;
  //printf("property name: %s, component name: %s, change mesh : %s \n ",p->name, c->name, name);
  Evas_Object* entry = evas_object_data_get(obj, "entry");
  elm_object_text_set(entry, name);


  const char* old = component_property_data_get(c, p);
  if (!strcmp(old, name)) {
    return;
  }

  component_property_data_set(c, p, &name);

  //TODO
  if (!strcmp(c->name, "mesh")) {
    MeshComponent* mc = c->data;
    if (!strcmp(p->name, "mesh"))
    mc->mesh = resource_mesh_get(s_rm, name);
    else if (!strcmp(p->name, "shader")) {
      mc->shader = resource_shader_get(s_rm, name);
    }
  }
}


static Evas_Object*
_create_resource_menu(Evas_Object* win, const char* resource_type)
{
  Evas_Object* menu;
  Elm_Object_Item *menu_it,*menu_it1;

  menu = elm_menu_add(win);

  Eina_Iterator* it;
  Eina_Hash* hash = NULL;
  void *data;

  if (!strcmp(resource_type, "mesh"))
  hash = resource_meshes_get(s_rm);
  else if (!strcmp(resource_type, "shader"))
  hash = resource_shaders_get(s_rm);

  if (!hash) return NULL;

  it = eina_hash_iterator_tuple_new(hash);

  while (eina_iterator_next(it, &data)) {
    Eina_Hash_Tuple *t = data;
    const char* name = t->key;
    //const Mesh* m = t->data;
    //printf("key, mesh name : %s, %s\n", name, m->name);
    elm_menu_item_add(menu, NULL, NULL, name, _change_resource, name);
  }
  eina_iterator_free(it);

  return menu;
}

static void
_entry_focused_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);
  const char* str = eina_stringshare_add(s);
  //TODO don't forget to eina_stringshare_del
  //
  double v =  elm_spinner_value_get(obj);
  printf("double get : %f \n", v);
  printf("TODO stringshare del\n");
  cp->value_saved = str;
}

static void
_spinner_drag_start_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  double v =  elm_spinner_value_get(obj);
  eina_value_setup(&cp->saved, EINA_VALUE_TYPE_DOUBLE);
  eina_value_set(&cp->saved, v);

  //TODO I have to know the original quaternion
  Property* p = evas_object_data_get(obj, "property");
  void* cd = cp->component->data;

  Quat q;
  int offset = property_offset_get(p);
  memcpy(&q, (void*)cd + offset, sizeof q);
  cp->quat_saved = q;
}

static void
_spinner_drag_stop_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  double v =  elm_spinner_value_get(obj);
}



static void
_entry_clicked_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  Property* p = evas_object_data_get(obj, "property");

  if (p->is_resource) {
    Evas_Object* win = evas_object_top_get(evas_object_evas_get(obj));
    Evas_Object* menu = _create_resource_menu(win, p->resource_type);
    if (!menu) return;
    evas_object_data_set(menu, "component", cp->component);
    evas_object_data_set(menu, "property", p);
    evas_object_data_set(menu, "entry", obj);
    evas_object_show(menu);

    Evas_Coord x,y,w,h;
    evas_object_geometry_get(obj, &x, &y, &w, &h);
    elm_menu_move(menu, x, y);
    
  }
  
}



static void
_entry_unfocused_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  const char* s = elm_object_text_get(obj);
  if (strcmp(cp->value_saved, s)) {
    cp->value_saved = s;
    Property* p = evas_object_data_get(obj, "property");
    control_property_change(cp->control, cp->component, p, cp->value_saved, s);
  }
}

static Evas_Object* 
_property_add_entry(ComponentProperties* cp, Property* p, void* data)
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
  if (p->is_resource)
  elm_entry_editable_set(en, EINA_FALSE);

  evas_object_name_set(en, p->name);

  printf("GET REAL PARENT of : %s \n", p->name);
  Property* parent = property_real_parent_get(p);
  if (parent && parent->type == EET_G_HASH) {
    /*
    printf("youuuuuuuuuuuuuuuuu I add a hash to the hash : %s \n", p->name);
    Eina_Hash* hash_prop = eina_hash_find(cp->properties, &parent);
    //TODO I must add something from data not from the property
    if (hash_prop) printf("I found the hash !!!!! I add %s \n", p->name);
    eina_hash_add(hash_prop, p->name, en);
    */
  }
  else {
    printf("just normal entry : %s \n", p->name);
    if (!p->parent)
    printf("it has no parent : %s \n", p->name);
    else
    printf("it has a parent : %s, parent name: %s, %d, %d \n", p->name, p->parent->name, p->parent->type, EET_G_HASH);
  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
        en);

  }

  //TODO
  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "parentdata", data );
  cp->entries = eina_list_append(cp->entries, en);

  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, cp);
  evas_object_smart_callback_add(en, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(en, "aborted", _entry_aborted_cb, cp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, cp);
  evas_object_smart_callback_add(en, "clicked", _entry_clicked_cb, cp);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(cp->box, bx2);
  evas_object_show(bx2);

  return en;

}


static Evas_Object* 
_property_add_spinner(ComponentProperties* cp, Property* p, Evas_Object* box, void* data)
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
  elm_spinner_editable_set(en, EINA_TRUE);

  char s[50];
  sprintf(s, "%s : %s", p->name, "%.4f");

  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, p->name);

  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
        en);

  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "parentdata", data );
  cp->entries = eina_list_append(cp->entries, en);
  printf("add spinner parent data : %p \n", data);

  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, cp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);

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

  //TODO 
  /*
  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
        en);
        */

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
_component_property_orientation_update(ComponentProperties* cp, void* data, Property* p)
{
  Quat q;
  int offset = property_offset_get(p);
  memcpy(&q, (void*)data + offset, sizeof q);
  Vec3 deg = quat_to_euler_deg(q);
  //printf("deg : %f, %f, %f \n", deg.x, deg.y, deg.z);
  Eina_List* widgets = eina_hash_find(cp->properties, &p);

  Eina_List* l;
  Evas_Object* o;
  int i = 0;
  EINA_LIST_FOREACH(widgets, l, o) {
    if (i==0)
    elm_spinner_value_set(o, deg.x );
    else if (i==1)
    elm_spinner_value_set(o, deg.y );
    else if (i==2)
    elm_spinner_value_set(o, deg.z );
    ++i;
  }
  
}

struct _ComponentPropertyCouple
{
  ComponentProperties* cp;
  Property* p;
  Evas_Object* box;
};

static Eina_Bool 
_component_property_update_hash(
      const Eina_Hash *hash,
      const void *key,
      void *data,
      void *fdata)
{

  const char* keyname = key;
  printf("update,the key is %s \n", keyname);
  Texture* t = data;

  struct _ComponentPropertyCouple* cpp = fdata;
  Eina_Hash* hash_prop = eina_hash_find(cpp->cp->properties, &cpp->p);
  Evas_Object* entry = eina_hash_find(hash_prop, keyname);
  elm_object_text_set(entry, t->filename );

  /*
  Evas_Object* entry = eina_hash_find(hash_prop, keyname);
  elm_object_text_set(entry, t->filename );
  */


  return EINA_TRUE;
}

static Evas_Object* 
_property_add_tex(ComponentProperties* cp, const char* name)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(cp->win);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(cp->win);
  char s[256];
  sprintf(s, "<b> %s </b> : ", name);

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

  elm_entry_editable_set(en, EINA_FALSE);

  evas_object_name_set(en, name);


  /*
  eina_hash_add(
        cp->properties,
        //p->name,
        &p,
        en);
        */

  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, cp);
  evas_object_smart_callback_add(en, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(en, "aborted", _entry_aborted_cb, cp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, cp);
  evas_object_smart_callback_add(en, "clicked", _entry_clicked_cb, cp);
  //evas_object_data_set(en, "property", p);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(cp->box, bx2);
  evas_object_show(bx2);

  return en;

}


static Eina_Bool 
_component_property_add_hash(
      const Eina_Hash *hash,
      const void *key,
      void *data,
      void *fdata)
{
  const char* keyname = key;
  struct _ComponentPropertyCouple* cpp = fdata;

  Evas_Object *label;

  label = elm_label_add(cpp->cp->win);
  char s[256];
  sprintf(s, "<b> %s </b> : ", keyname);

  elm_object_text_set(label, s);
  elm_box_pack_end(cpp->cp->box, label);
  evas_object_show(label);

  _add_properties(cpp->cp, cpp->p->sub, cpp->box, data);
  

  return EINA_TRUE;

  Evas_Object* o = _property_add_tex(cpp->cp, keyname);
  Eina_Hash* hash_prop = eina_hash_find(cpp->cp->properties, &cpp->p);
  eina_hash_add(hash_prop, keyname, o);

  return EINA_TRUE;
}



static void
_component_property_update_data_recur(ComponentProperties* cp, void* data, const Property* ps)
{
  Property *p;
  Eina_List* l;
  EINA_LIST_FOREACH(ps->list, l, p) {
    Evas_Object* obj = eina_hash_find(cp->properties, &p);
    printf("component %s, name: %s , type: %d, offset: %d\n", cp->name, p->name, p->type, p->offset);
    switch(p->type) {
      case EET_T_DOUBLE:
         {
          double d;
          int offset = property_offset_get(p);
          memcpy(&d, (void*)data + offset, sizeof d);
          //printf("my value is : %f\n", d);
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
          int offset = property_offset_get(p);
          const char** str = (void*)data + offset;
          const char* s = elm_object_text_get(obj);
          if (!*str) break;
          if (strcmp(*str,s)) 
            elm_object_text_set(obj, *str );
         }
        break;
     case PROPERTY_STRUCT:
         {
          int offset = property_offset_get(p);
          void** structdata = (void*)data + offset;
          _component_property_update_data_recur(cp, *structdata, p->sub);
         }
         break;
     case PROPERTY_STRUCT_NESTED:
        if  (!strcmp(p->name, "orientation")) {
          _component_property_orientation_update(cp, data, p);
        }
        else
        _component_property_update_data_recur(cp, data, p->sub);
         break;
      case EET_G_HASH:
          {
           //Eina_Hash* hash_prop = eina_hash_find(cp->properties, &p);
           int offset = property_offset_get(p);
           const void** ptr = (void*)data + offset;
           const Eina_Hash* hash = *ptr;
           struct _ComponentPropertyCouple cpp = {cp, p};
           eina_hash_foreach(hash, _component_property_update_hash, &cpp);
          }
         break;
     case PROPERTY_POINTER:
         continue;
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
component_property_update_data(ComponentProperties* cp, void* datacaca)
{
  //_component_property_update_data_recur(cp, datacaca, cp->property);
  //return;

  Eina_List* l;
  Evas_Object* obj;
  EINA_LIST_FOREACH(cp->entries, l, obj) {
    Property* p = evas_object_data_get(obj, "property");
    void* data = evas_object_data_get(obj, "parentdata");

    switch(p->type) {
      case EET_T_DOUBLE:
         {
          double d;
          int offset = property_offset_get(p);
          memcpy(&d, (void*)data + offset, sizeof d);
          //printf("my value is : %f\n", d);
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
          int offset = property_offset_get(p);
          const char** str = (void*)data + offset;
          const char* s = elm_object_text_get(obj);
          if (!*str) break;
          if (strcmp(*str,s)) 
            elm_object_text_set(obj, *str );
         }
        break;
    }
  }
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
  control_component_remove(cp->control, cp->pw->context->object, cp->component);
}

static Evas_Object* 
_property_add_spinner_angle(
      ComponentProperties* cp,
      Property* p,
      Evas_Object* box,
      const char* name)
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
  elm_spinner_editable_set(en, EINA_TRUE);

  char s[50];
  sprintf(s, "%s : %s", name, "%.4f");

  elm_spinner_label_format_set(en, s);

  evas_object_name_set(en, name);

  evas_object_smart_callback_add(en, "changed", _entry_orientation_changed_cb, cp);
  evas_object_smart_callback_add(en, "spinner,drag,start", _spinner_drag_start_cb, cp);
  evas_object_smart_callback_add(en, "spinner,drag,stop", _spinner_drag_stop_cb, cp);

  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "property_name", name);

  return en;
}


static void
_add_orientation_properties(ComponentProperties* cp, Property* p, Evas_Object* box)
{
  Eina_List* l = NULL;

  l = eina_list_append(l,
  _property_add_spinner_angle(cp, p, box, "x"));
  l = eina_list_append(l,
  _property_add_spinner_angle(cp, p, box, "y"));
  l = eina_list_append(l,
  _property_add_spinner_angle(cp, p, box, "z"));

  eina_hash_add(
        cp->properties,
        &p,
        l);

}

static Evas_Object*
_property_struct_add(ComponentProperties* cp, const Property* p)
{
  Evas_Object *label;

  label = elm_label_add(cp->win);
  char s[256];
  sprintf(s, "<b> %s </b> : ", p->name);

  elm_object_text_set(label, s);
  elm_box_pack_end(cp->box, label);
  evas_object_show(label);

  printf("youuuuuuuuuuuuuuuuu I add a hash to the hash : %s \n", p->name);
  Eina_Hash* hash_prop = eina_hash_find(cp->properties, &p);
  //TODO I must add something from data not from the property
  //if (hash_prop) printf("I found the hash !!!!! I add %s \n", p->name);
  //eina_hash_add(hash_prop, p->name, en);

  //evas_object_data_set(en, "property", p);

  return label;
}


static void
_add_properties(ComponentProperties* cp, const Property* ps, Evas_Object* box, void* data )
{
  Property *p;
  Eina_List* l;
  EINA_LIST_FOREACH(ps->list, l, p) {
   //printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
   //printf("   value is : ");
   switch(p->type) {
     case EET_T_DOUBLE:
        {
         _property_add_spinner(cp, p, box, data);
        }
         break;
     case EET_T_STRING:
         _property_add_entry(cp, p, data);
         break;
     case PROPERTY_FILENAME:
         _property_add_fileselect(cp, p);
         break;
     case PROPERTY_STRUCT:
          {
           int offset = property_offset_get(p);
           void** datastruct = (void*)data + offset;
           _property_struct_add(cp, p);
           //TODO do something like this?
           //ComponentProperties* cpsub=
           // create_my_prop(Component* c, Evas_Object* win, Control* control, bool can_remove)
           _add_properties(cp, p->sub, cp->box, *datastruct);
          }
         break;
     case PROPERTY_STRUCT_NESTED:
         if (p->sub->hint == HORIZONTAL) {
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

           if (!strcmp(p->name, "orientation")) {
             _add_orientation_properties(cp, p, hbox);
           }
           else
           _add_properties(cp, p->sub, hbox, data);

         }
         //else
         //_add_properties(cp, p->sub, cp->box, data);
         break;
     case EET_G_HASH:
          {
           Eina_Hash* hash_prop = eina_hash_string_superfast_new(NULL);
           //TODO chris now
           eina_hash_add(
                 cp->properties,
                 &p,
                 hash_prop);

           printf("add properties we have a hash\n");
           int offset = property_offset_get(p);
           const void** ptr = (void*)data + offset;
           const Eina_Hash* hash = *ptr;
           struct _ComponentPropertyCouple cpp = {cp, p, box};
           eina_hash_foreach(hash, _component_property_add_hash, &cpp);
           //_add_properties(cp, p->sub, box, hash);
          }
         break;
     case PROPERTY_POINTER:
         //TODO
         //_property_add_entry(cp, p);
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
  cp->property = c->properties;
  cp->win = win;
  cp->control = control;
  //cp->properties = eina_hash_string_superfast_new(_property_entry_free_cb);
  cp->properties = eina_hash_pointer_new(_property_entry_free_cb);
  cp->name = c->name;

  Evas_Object* frame = elm_frame_add(win);
  elm_frame_autocollapse_set(frame, EINA_TRUE);
  char s[256];
  sprintf(s, "Component/%s", c->name);
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

  _add_properties(cp, c->properties, cp->box, c->data);

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

//TODO chris
ComponentProperties*
create_my_prop_data(Component* c, Evas_Object* win, Control* control, Property* p, void* data)
{
  ComponentProperties* cp = calloc(1, sizeof *cp);
  cp->component = c;
  cp->property = p;
  cp->win = win;
  cp->control = control;
  cp->name = p->name;

  Evas_Object* frame = elm_frame_add(win);
  elm_frame_autocollapse_set(frame, EINA_TRUE);
  char s[256];
  sprintf(s, "Component/%s", c->name);
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

  _add_properties(cp, c->properties, cp->box, c->data);

  return cp;
}

