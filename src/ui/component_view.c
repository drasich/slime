#include <Elementary.h>
#include <Eina.h>
#include "ui/property_view.h"
#include "property.h"
#include "view.h"
#include "resource.h"
#include "component/meshcomponent.h" //TODO remove

static void _add_properties(
      ComponentProperties* cp,
      const Property* ps,
      Evas_Object* box,
      void* data);

static void
_entry_orientation_changed_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;

  Property* p = evas_object_data_get(obj, "property");
  void* thedata = evas_object_data_get(obj, "data");
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

  cp->control->rotation = q;

  q = quat_mul(cp->quat_saved, q);
  int offset = property_offset_get(p);
  memcpy(thedata + offset, &q, sizeof q);
}

static void
_entry_vec3_changed_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;

  Property* p = evas_object_data_get(obj, "property");
  void* thedata = evas_object_data_get(obj, "data");
  const char* name = evas_object_data_get(obj, "property_name");

  double f =  elm_spinner_value_get(obj);
  /*
  double saved;
  eina_value_get(&cp->saved, &saved);
  saved = ;
  */

  int offset = property_offset_get(p);
  Vec3* v = thedata + offset;

  if (!strcmp(name, "x")) {
    v->x = f;
  }
  else if (!strcmp(name, "y")) {
    v->y = f;
  }
  else if (!strcmp(name, "z")) {
    v->z = f;
  }

  //memcpy(thedata + offset, &q, sizeof q);
}

static void
_entry_changed_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;

  Property* p = evas_object_data_get(obj, "property");
  void* thedata = evas_object_data_get(obj, "data");

  switch(p->type) {
    case EET_T_DOUBLE:
       {
        double v =  elm_spinner_value_get(obj);
        int offset = property_offset_get(p);
        memcpy(thedata + offset, &v, p->size);
       }
      break;
    case EET_T_STRING:
       {
        int offset = property_offset_get(p);
        const char** str = (void*)thedata + offset;
        eina_stringshare_del(*str);
        const char* s = elm_object_text_get(obj);
        *str = eina_stringshare_add(s);
        //eina_stringshare_dump();
       }
      break;
    case PROPERTY_UNIFORM:
       {
        UniformValue* uv = thedata;
        if (uv->type == UNIFORM_FLOAT) {
          float f =  elm_spinner_value_get(obj);
          int offset = property_offset_get(p);
          uv->value.f = f;
        }
       }
      break;
    default:
      fprintf (stderr, "type not yet implemented: at %s, line %d\n",__FILE__, __LINE__);
      break;
   }

  Control* ct = cp->control;

  if (cp->callback) {
    cp->callback(ct, thedata, p);
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

  printf("entry activated\n");
  Property* p = evas_object_data_get(obj, "property");
  void *thedata = evas_object_data_get(obj, "data");

  if (p->type == EET_T_STRING) {
    const char* s = elm_object_text_get(obj);
    if (strcmp(cp->value_saved, s)) {
      control_property_change(cp->control, cp->component, thedata, p, cp->value_saved, s);
      cp->value_saved = s;
    }
  }
  else if (p->type == EET_T_DOUBLE) {
    const char* s = elm_object_text_get(obj);
    double v = atof(s);
    double saved;
    eina_value_get(&cp->saved, &saved);
    if (saved != v) {
      printf("activated send the change %f, %f\n", saved, v);
      double *old = malloc(sizeof *old);
      eina_value_get(&cp->saved, old);
      double *new = malloc(sizeof *new);
      *new = v;
      control_property_change(cp->control, cp->component, thedata, p, old, new);
    }
  }
  else if (p->type == PROPERTY_QUAT) {
    Quat *old = malloc(sizeof *old);
    *old = cp->quat_saved;

    const char* name = evas_object_data_get(obj, "property_name");

    const char* s = elm_object_text_get(obj);
    double v = atof(s);
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

    Quat *new = malloc(sizeof *new);
    *new = quat_mul(*old, q);

    if (!quat_equal(*old, *new)) {
      control_property_change(cp->control, cp->component, thedata, p, old, new);
    }
    else {
      free(old);
      free(new);
    }

  }
  /*
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
  */

}

static void
_entry_aborted_cb(void *data, Evas_Object *obj, void *event)
{
  printf("aborted\n");

  ComponentProperties* cp = data;
  Component* component = cp->component;
  const char* s = elm_object_text_get(obj);

  Property* p = evas_object_data_get(obj, "property");
  void *thedata = evas_object_data_get(obj, "data");

  if (p->type == EET_T_STRING) {
    if (strcmp(cp->value_saved, s)) {
      eina_stringshare_del(s);

      int offset = property_offset_get(p);
      const char** str = (void*)thedata + offset;
      *str = eina_stringshare_add(cp->value_saved);
      elm_object_text_set(obj, *str);

      Control* ct = cp->control;
      control_property_update(ct, component);
    }
  }
  else if (p->type == EET_T_DOUBLE) {
    /*
    double saved;
    eina_value_get(&cp->saved, &saved);
    double d = atof(s);
    if (d != saved) {
      //elm_object_text_set(obj, *str);
      memcpy(thedata + p->offset, &d, sizeof(double));

      Control* ct = cp->control;
      control_property_update(ct, component);
    }
    */
  }

}

static void
_change_resource(void *data,
      Evas_Object *obj,
      void *event_info)
{
  Property* p = evas_object_data_get(obj, "property");
  ComponentProperties* cp = evas_object_data_get(obj, "componentproperties");
  const char *name = data;
  //printf("property name: %s, component name: %s, change resource : %s \n ",p->name, c->name, name);
  Evas_Object* entry = evas_object_data_get(obj, "entry");
  elm_object_text_set(entry, name);

  Property* entryproperty = evas_object_data_get(entry, "property");
  void* entrydata = evas_object_data_get(entry, "data");

  int offset = property_offset_get(entryproperty);
  void** theolddata  = (void*)(entrydata + offset);
  const char* oldstr = *theolddata;
  if (oldstr && !strcmp(oldstr, name)) {
    return;
  }

  ResourceHandle* rh = entrydata;
  ResourceHandle* old = malloc(sizeof *old);
  ResourceHandle* new = malloc(sizeof *new);
  memcpy(old, entrydata, sizeof(*old));

  if (p->resource_type == RESOURCE_TEXTURE) {
    resource_texture_handle_set(s_rm, rh, name);

  }
  else if( p->resource_type == RESOURCE_MESH ) {
    resource_mesh_handle_set(s_rm, rh, name);
  }
  else if( p->resource_type == RESOURCE_SHADER ) {
    resource_shader_handle_set(s_rm, rh, name);
    property_reload_component(cp->pw, cp->component);
  }
  else{
    free(old);
    free(new);
    return;
  }

  memcpy(new, entrydata, sizeof(*new));
  control_property_change(cp->control, cp->component, entrydata, p, old, new);
}

static Evas_Object*
_create_resource_menu(Evas_Object* win, ResourceType resource_type)
{
  Evas_Object* menu;
  Elm_Object_Item *menu_it,*menu_it1;

  menu = elm_menu_add(win);

  Eina_Iterator* it;
  Eina_Hash* hash = NULL;
  void *data;

  switch(resource_type) {
    case RESOURCE_MESH:
      hash = resource_meshes_get(s_rm);
      break;
    case RESOURCE_SHADER:
      hash = resource_shaders_get(s_rm);
      break;
    case RESOURCE_TEXTURE:
      hash = resource_textures_get(s_rm);
      break;
  }

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

  Property* p = evas_object_data_get(obj, "property");
  if (p->type == EET_T_DOUBLE || p->type == PROPERTY_QUAT) {
    const char* s = elm_object_text_get(obj);
    double v = atof(s);
    eina_value_setup(&cp->saved, EINA_VALUE_TYPE_DOUBLE);
    eina_value_set(&cp->saved, v);

    if (p->type == PROPERTY_QUAT) {
      void* thedata = evas_object_data_get(obj, "data");
      Quat q;
      int offset = property_offset_get(p);
      memcpy(&q, thedata + offset, sizeof q);
      cp->quat_saved = q;
    }

  }
  else if (p->type == EET_T_STRING) {
    const char* s = elm_object_text_get(obj);
    const char* str = eina_stringshare_add(s);
    //printf("TODO stringshare del\n");
    //TODO don't forget to eina_stringshare_del
    //
    cp->value_saved = str;
  }
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
  void* thedata = evas_object_data_get(obj, "data");

  Quat q;
  int offset = property_offset_get(p);
  memcpy(&q, thedata + offset, sizeof q);
  cp->quat_saved = q;
}

static void
_spinner_drag_stop_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  double v =  elm_spinner_value_get(obj);
  Property* p = evas_object_data_get(obj, "property");
  void* thedata = evas_object_data_get(obj, "data");
  if (p->type == EET_T_DOUBLE){
    double *old = malloc(sizeof *old);
    eina_value_get(&cp->saved, old);
    double *new = malloc(sizeof *new);
    *new = v;
    control_property_change(cp->control, cp->component, thedata, p, old, new);
  }
  else if (p->type == PROPERTY_QUAT) {
    Quat *old = malloc(sizeof *old);
    //TODO use eina_value
    //eina_value_get(&cp->saved, old);
    *old = cp->quat_saved;
    Quat *new = malloc(sizeof *new);
    memcpy(new, thedata+p->offset, sizeof*new);
    control_property_change(cp->control, cp->component, thedata, p, old, new);
  }
  else if (p->type == PROPERTY_UNIFORM) {

    //TODO uniform undo/redo
    /*
    int offset = property_offset_get(p);
    UniformValue* uv = thedata + offset;
    if (uv->type == UNIFORM_FLOAT) {
      double *old = malloc(sizeof *old);
      eina_value_get(&cp->saved, old);
      double *new = malloc(sizeof *new);
      *new = v;
      control_property_change(cp->control, cp->component, thedata, p, old, new);
    }
    */

  }

}

static void
_entry_clicked_cb(void *data, Evas_Object *obj, void *event)
{
  ComponentProperties* cp = data;
  Property* p = evas_object_data_get(obj, "property");

  if (p->type == PROPERTY_RESOURCE) {
    Evas_Object* win = evas_object_top_get(evas_object_evas_get(obj));
    Evas_Object* menu = _create_resource_menu(win, p->resource_type);
    if (!menu) return;
    evas_object_data_set(menu, "componentproperties", cp);
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
  Property* p = evas_object_data_get(obj, "property");
  void* thedata = evas_object_data_get(obj, "data");

  if (p->type == EET_T_STRING) {
    const char* s = elm_object_text_get(obj);
    if (strcmp(cp->value_saved, s)) {
      control_property_change(cp->control, cp->component, thedata, p, cp->value_saved, s);
    }
  }
  else if (p->type == EET_T_DOUBLE) {
    const char* s = elm_object_text_get(obj);
    double v = atof(s);
    //double v =  elm_spinner_value_get(obj);
    double saved;
    eina_value_get(&cp->saved, &saved);
    printf("entry unfocused, it's double: %f to %f\n", saved, v);
    if (saved != v) {
      printf("send the change %f, %f\n", saved, v);
      double *old = malloc(sizeof *old);
      eina_value_get(&cp->saved, old);
      double *new = malloc(sizeof *new);
      *new = v;
      control_property_change(cp->control, cp->component, thedata, p, old, new);
    }
  }
}

static Evas_Object* 
_property_add_entry(ComponentProperties* cp, const Property* p, void* data, Evas_Object* box)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(cp->win);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  if (!p->hide_name) {
    label = elm_label_add(cp->win);
    char s[256];
    sprintf(s, "<b> %s </b> : ", p->name);

    elm_object_text_set(label, s);
    evas_object_show(label);
    elm_box_pack_end(bx2, label);
  }

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
  if (p->type == PROPERTY_RESOURCE)
  elm_entry_editable_set(en, EINA_FALSE);

  evas_object_name_set(en, p->name);

  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "data", data );
  cp->entries = eina_list_append(cp->entries, en);

  evas_object_smart_callback_add(en, "changed,user", _entry_changed_cb, cp);
  evas_object_smart_callback_add(en, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(en, "aborted", _entry_aborted_cb, cp);
  evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, cp);
  evas_object_smart_callback_add(en, "clicked", _entry_clicked_cb, cp);

  elm_entry_context_menu_disabled_set(en, EINA_TRUE);
  
  elm_box_pack_end(box, bx2);
  evas_object_show(bx2);

  return en;

}

static Evas_Object* 
_property_add_spinner(ComponentProperties* cp, const Property* p, Evas_Object* box, void* data)
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
  if (p->name) {
    evas_object_name_set(en, p->name);
    sprintf(s, "%s : %s", p->name, "%.4f");
  }
  else
    sprintf(s, "%s", "%.4f");

  elm_spinner_label_format_set(en, s);

  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "data", data );
  cp->entries = eina_list_append(cp->entries, en);

  evas_object_smart_callback_add(en, "changed", _entry_changed_cb, cp);
  //evas_object_smart_callback_add(en, "focused", _entry_focused_cb, cp);
  //evas_object_smart_callback_add(en, "unfocused", _entry_unfocused_cb, cp);
  evas_object_smart_callback_add(en, "spinner,drag,start", _spinner_drag_start_cb, cp);
  evas_object_smart_callback_add(en, "spinner,drag,stop", _spinner_drag_stop_cb, cp);

  Evas_Object* entry = elm_layout_content_get(en, "elm.swallow.entry");
  evas_object_data_set(entry, "property", p);
  evas_object_data_set(entry, "data", data );

  evas_object_smart_callback_add(entry, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(entry, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(entry, "unfocused", _entry_unfocused_cb, cp);
  //evas_object_smart_callback_add(entry, "aborted", _entry_aborted_cb, cp);

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
_property_add_fileselect(ComponentProperties* cp, const Property* p)
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

struct _ComponentPropertyCouple
{
  ComponentProperties* cp;
  const Property* p;
  Evas_Object* box;
};


static Eina_Bool 
_component_property_add_hash(
      const Eina_Hash *hash,
      const void *key,
      void *data,
      void *fdata)
{
  const char* keyname = key;
  struct _ComponentPropertyCouple* cpp = fdata;

  if (cpp->p->sub->type == PROPERTY_RESOURCE) {
    Evas_Object* bx;
    bx = elm_box_add(cpp->cp->win);
    elm_box_horizontal_set(bx, EINA_TRUE);
    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

    Evas_Object *label;

    label = elm_label_add(cpp->cp->win);
    char s[256];
    sprintf(s, "<b> %s </b> : ", keyname);

    elm_object_text_set(label, s);
    elm_box_pack_end(bx, label);
    evas_object_show(label);

    elm_box_pack_end(cpp->box, bx);
    evas_object_show(bx);

    _add_properties(cpp->cp, cpp->p->sub, bx, data);
  }
  else if (cpp->p->sub->type == PROPERTY_UNIFORM) {
    Evas_Object* bx;
    bx = elm_box_add(cpp->cp->win);
    elm_box_horizontal_set(bx, EINA_TRUE);
    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

    Evas_Object *label;

    label = elm_label_add(cpp->cp->win);
    char s[256];
    sprintf(s, "<b> %s </b> : ", keyname);

    elm_object_text_set(label, s);
    elm_box_pack_end(bx, label);
    evas_object_show(label);

    elm_box_pack_end(cpp->box, bx);
    evas_object_show(bx);

    _add_properties(cpp->cp, cpp->p->sub, bx, data);
  }

  return EINA_TRUE;
}

void
component_property_update_data(ComponentProperties* cp)
{
  Eina_List* l;
  Evas_Object* obj;
  EINA_LIST_FOREACH(cp->entries, l, obj) {
    Property* p = evas_object_data_get(obj, "property");
    void* data = evas_object_data_get(obj, "data");
    //printf("update data property %s, data %p, type %d \n", p->name, data, p->type);

    switch(p->type) {
      case PROPERTY_QUAT:
         {
          Quat q;
          int offset = property_offset_get(p);
          memcpy(&q, data + offset, sizeof q);
          Vec3 deg = quat_to_euler_deg(q);
          const char* pname = evas_object_data_get(obj, "property_name");
          if (!strcmp(pname, "x"))
          elm_spinner_value_set(obj, deg.x );
          else if (!strcmp(pname, "y"))
          elm_spinner_value_set(obj, deg.y );
          else if (!strcmp(pname, "z"))
          elm_spinner_value_set(obj, deg.z );
         }
        break;
      case EET_T_DOUBLE:
         {
          double d;
          int offset = property_offset_get(p);
          //memcpy(&d, (void*)data + offset, sizeof d);
          memcpy(&d, data + offset, sizeof d);
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
          //const char** str = (void*)data + offset;
          const char** str = data + offset;
          const char* s = elm_object_text_get(obj);
          if (!*str) break;
          if (strcmp(*str,s)) 
            elm_object_text_set(obj, *str );
         }
        break;
     case PROPERTY_POINTER:
         /*
         continue;
          {
          int offset = property_offset_get(p);
          const void** ptr = (void*)data + offset;
          const char* s = elm_object_text_get(obj);
          const Object* o = *ptr;

          if (!o) break;
          if (strcmp(o->name,s)) 
            elm_object_text_set(obj, o->name );

          }
          */

         break;
     case PROPERTY_RESOURCE:
          {
           int offset = property_offset_get(p);
           const ResourceHandle* rh = data + offset;
           const char* s = elm_object_text_get(obj);
           if (rh && rh->name && strcmp(rh->name,s)) 
           elm_object_text_set(obj, rh->name );
          }
         break;
     case PROPERTY_UNIFORM:
          {
           int offset = property_offset_get(p);
           UniformValue* uv = data + offset;
           if (uv->type == UNIFORM_FLOAT) {
             float f = uv->value.f;
             float old = elm_spinner_value_get(obj);
             if (old != f) {
               elm_spinner_value_set(obj, f);
             }
           }
           else if (uv->type == UNIFORM_VEC3) {
             Vec3 v = uv->value.vec3;
             float old = elm_spinner_value_get(obj);
             const char* pname = evas_object_data_get(obj, "property_name");
             if (!strcmp(pname, "x"))
             elm_spinner_value_set(obj, v.x );
             else if (!strcmp(pname, "y"))
             elm_spinner_value_set(obj, v.y );
             else if (!strcmp(pname, "z"))
             elm_spinner_value_set(obj, v.z );
           }
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
      const Property* p,
      Evas_Object* box,
      const char* name,
      void* data)
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
  //todo chris


  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "property_name", name);
  evas_object_data_set(en, "data", data );
  cp->entries = eina_list_append(cp->entries, en);

  Evas_Object* entry = elm_layout_content_get(en, "elm.swallow.entry");
  evas_object_data_set(entry, "property", p);
  evas_object_data_set(entry, "property_name", name);
  evas_object_data_set(entry, "data", data );

  evas_object_smart_callback_add(entry, "activated", _entry_activated_cb, cp);
  evas_object_smart_callback_add(entry, "focused", _entry_focused_cb, cp);
  evas_object_smart_callback_add(entry, "unfocused", _entry_unfocused_cb, cp);

  return en;
}

static Evas_Object* 
_property_add_spinner_vec3(
      ComponentProperties* cp,
      const Property* p,
      Evas_Object* box,
      const char* name,
      void* data)
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

  evas_object_smart_callback_add(en, "changed", _entry_vec3_changed_cb, cp);
  //evas_object_smart_callback_add(en, "spinner,drag,start", _spinner_drag_start_cb, cp);
  //evas_object_smart_callback_add(en, "spinner,drag,stop", _spinner_drag_stop_cb, cp);


  evas_object_data_set(en, "property", p);
  evas_object_data_set(en, "property_name", name);
  evas_object_data_set(en, "data", data );
  cp->entries = eina_list_append(cp->entries, en);

  return en;
}




static void
_add_orientation_properties(ComponentProperties* cp, const Property* p, Evas_Object* box, void* data)
{
  Eina_List* l = NULL;

  l = eina_list_append(l,
  _property_add_spinner_angle(cp, p, box, "x", data));
  l = eina_list_append(l,
  _property_add_spinner_angle(cp, p, box, "y", data));
  l = eina_list_append(l,
  _property_add_spinner_angle(cp, p, box, "z", data));
}

static void
_property_vec3_add(ComponentProperties* cp, const Property* p, Evas_Object* box, void* data)
{
  _property_add_spinner_vec3(cp, p, box, "x", data);
  _property_add_spinner_vec3(cp, p, box, "y", data);
  _property_add_spinner_vec3(cp, p, box, "z", data);
}


static Evas_Object*
_property_struct_add(ComponentProperties* cp, const Property* p, Evas_Object* box_parent)
{
  /*
  Evas_Object *label;

  label = elm_label_add(cp->win);
  char s[256];
  sprintf(s, ".:: <b> %s </b> ::.", p->name);
  evas_object_size_hint_align_set(label, 0,0);

  elm_object_text_set(label, s);
  elm_box_pack_end(cp->box, label);
  evas_object_show(label);

  return label;
  */
  //return NULL;

  Evas_Object* frame = elm_frame_add(cp->win);
  elm_frame_autocollapse_set(frame, EINA_TRUE);
  char s[256];
  sprintf(s, "%s", p->name);
  elm_object_text_set(frame, s);
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  elm_box_pack_end(box_parent, frame);
  evas_object_show(frame);

  Evas_Object* box = elm_box_add(cp->win);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_align_set(box, 0.0, 0.0);
  //evas_object_show(cp->box);
  elm_object_content_set(frame, box);

  return box;
}

static void
_property_add(ComponentProperties* cp, const Property* p, Evas_Object* box, void* data)
{
  if (!data) {
    return;
  }
  //printf("property add name: %s , type: %d, offset: %d, data : %p\n", p->name, p->type, p->offset, data);

  switch(p->type) {
    case EET_T_DOUBLE:
       {
        _property_add_spinner(cp, p, box, data);
       }
      break;
    case EET_T_STRING:
      _property_add_entry(cp, p, data, box);
      break;
    case PROPERTY_FILENAME:
      _property_add_fileselect(cp, p);
      break;
    case PROPERTY_STRUCT:
       {
        int offset = property_offset_get(p);
        void** datastruct = data + offset;
        Evas_Object* bx = _property_struct_add(cp, p, box);
        _add_properties(cp, p->sub, bx, *datastruct);
       }
      break;
    case PROPERTY_QUAT:
        {
          _add_orientation_properties(cp, p, box, data);
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

        int offset = property_offset_get(p);
        void* datastruct = data + offset;
        _add_properties(cp, p->sub, hbox, datastruct);

      }
      else {
        int offset = property_offset_get(p);
        void* datastruct = (void*)data + offset;
        _add_properties(cp, p->sub, cp->box, datastruct);
      }
      break;
    case EET_G_HASH:
       {
        int offset = property_offset_get(p);
        const void** ptr = data + offset;
        const Eina_Hash* hash = *ptr;
        struct _ComponentPropertyCouple cpp = {cp, p, box};
        eina_hash_foreach(hash, _component_property_add_hash, &cpp);
       }
      break;
    case PROPERTY_POINTER:
      //TODO
      //_property_add_entry(cp, p);
      break;
    case PROPERTY_RESOURCE:
      _property_add_entry(cp, p, data, box);
      break;
    case PROPERTY_UNIFORM:
       {
        int offset = property_offset_get(p);
        UniformValue* uv = data + offset;
        //printf("this is a uniform, type is %d \n", uv->type);
        if (uv->type == UNIFORM_FLOAT) { 
          //printf("this is a uniform, type is float !! %d \n", uv->type);
          _property_add_spinner(cp, p, box, data);
        }
        else if (uv->type == UNIFORM_VEC3) { 
          //printf("this is a uniform, type is vec3 !! %d \n", uv->type);
          _property_vec3_add(cp, p, box, data);
          //_property_add_spinner(cp, p, box, data);
        }
       }
      break;
    case PROPERTY_ROOT:
      break;
    default:
      fprintf (stderr, "type '%d' not yet implemented: at %s, line %d\n", p->type, __FILE__, __LINE__);
      break;
  }

}

static void
_add_properties(ComponentProperties* cp, const Property* ps, Evas_Object* box, void* data )
{
  _property_add(cp, ps, box, data);

  if (ps->type == PROPERTY_QUAT) return;

  Property *p;
  Eina_List* l;
  EINA_LIST_FOREACH(ps->list, l, p) {
    //printf("in list name: %s , type: %d, offset: %d, data : %p\n", p->name, p->type, p->offset, data);
    _property_add(cp, p, box, data);
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

