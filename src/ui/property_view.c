#include <Elementary.h>
#include <Eina.h>
#include "ui/property_view.h"
#include "property.h"

void property_clear_components(PropertyView* pw)
{
  elm_box_unpack_all(pw->box);

  Eina_List* l;
  ComponentProperties* cp;

  EINA_LIST_FOREACH(pw->component_widgets, l, cp) {
    evas_object_hide(cp->root);
  }

  pw->component_widgets = eina_list_free(pw->component_widgets);
  //TODO clean the components here
  printf("TODO clean components view here\n");
}

void property_reload_component(PropertyView* pw, Component* c)
{
  ComponentProperties* cp = create_component_properties(c, pw, true);
  cp->component = c;
  component_property_update_data(cp);

  Eina_List* l;
  ComponentProperties* icp;
  ComponentProperties* found;
  EINA_LIST_FOREACH(pw->component_widgets, l, icp) {
    if (icp->component == c) {
      elm_box_pack_after(pw->box, cp->root, icp->root);
      elm_box_unpack(pw->box, icp->root);
      found = icp;
      evas_object_hide(icp->root);
      evas_object_show(cp->root);
      break;
    }
  }
  
  if (found) {
    pw->component_widgets = eina_list_append_relative(pw->component_widgets, cp, found);
    pw->component_widgets = eina_list_remove(pw->component_widgets, found);
  }

}

void
property_add_component(PropertyView* pw, ComponentProperties* cp)
{
  elm_box_pack_end(pw->box, cp->root);
  evas_object_show(cp->root);
  pw->component_widgets = eina_list_append(pw->component_widgets, cp);
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
    v = vec3_add(v, o->position);
    last = o;
    i++;
  }

  int nb = eina_list_count(objects);
  if (nb == 1) {
    if (last != pw->object) {
      pw->object = last;
      property_object_show(pw, last);
    }
  }
  else {
    if (pw->object) {
      pw->object = NULL;
      property_clear_components(pw);
    }
    //if (nb > 1) {
    //property_set(pw, NULL);
    //_property_update_data(pw->current, &pw->context->mos);
  }
}

void
property_scene_show(PropertyView* pw, Scene* s)
{
  property_clear_components(pw);
  elm_box_unpack_all(pw->box_parent);
  evas_object_hide(pw->box_prefab_info);
  elm_box_pack_end(pw->box_parent, pw->box);
  evas_object_show(pw->box);


  ComponentProperties* cp = create_data_properties(s, property_set_scene(), pw, false);
  property_add_component(pw, cp);
  printf("todo chris : %s \n", s->name);
  component_property_update_data(cp);
}

void
property_update_components_data(PropertyView* pw, Component* component)
{
  Eina_List* l;
  ComponentProperties* cp;

  EINA_LIST_FOREACH(pw->component_widgets, l, cp) {
    if (component == cp->component)
    component_property_update_data(cp);
  }
}


/*
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
*/

void
property_clean(PropertyView* pw)
{
  elm_box_clear(pw->box);
}

void
property_set(PropertyView* pw, ComponentProperties* cp)
{
  if (cp) {
    //elm_object_content_set(pw->scroller, cp->box);
    //evas_object_show(cp->box);
    elm_box_pack_end(pw->box, cp->box);
    evas_object_show(cp->box);
  }
}

static Property* 
_multiple_objects_init_array_properties()
{
  Property* ps = property_set_new();

  /*
  ADD_PROP(ps, Multiple_Objects_Selection, center.x, EET_T_DOUBLE);
  ADD_PROP(ps, Multiple_Objects_Selection, center.y, EET_T_DOUBLE);
  ADD_PROP(ps, Multiple_Objects_Selection, center.z, EET_T_DOUBLE);
  ADD_PROP(ps, Multiple_Objects_Selection, rotation.x, EET_T_DOUBLE);
  ADD_PROP(ps, Multiple_Objects_Selection, rotation.y, EET_T_DOUBLE);
  ADD_PROP(ps, Multiple_Objects_Selection, rotation.z, EET_T_DOUBLE);
  */

  return ps;
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
  property_update(propertyview, c->objects);
}


static void
_property_entry_free_cb(void *data)
{
   //free(data);
}

static Evas_Object*
_prefab_prop_create(PropertyView* pw, Evas_Object* bx)
{
  Evas_Object *en, *bx2, *label;

  bx2 = elm_box_add(bx);
  elm_box_horizontal_set(bx2, EINA_TRUE);
  evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(bx);
  elm_object_text_set(label, "<b> Prefab linked </b> : ");
  evas_object_show(label);
  elm_box_pack_end(bx2, label);

  en = elm_entry_add(bx);
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

  elm_box_pack_end(bx, bx2);
  evas_object_show(bx2);

  evas_object_data_set(bx, "entry", en);

  Evas_Object* bt = elm_button_add(bx);
  elm_object_text_set(bt, "unlink");
  evas_object_show(bt);
  elm_box_pack_end(bx2, bt);
  evas_object_data_set(bx, "button", bt);
  evas_object_data_set(bt, "propertyview", pw);

  //property_holder_object_add(data + p->offset, p, en);

  return en;

}


PropertyView* 
create_property(Evas_Object* win, Context* context, Control* control)
{
  PropertyView *p = calloc(1, sizeof *p);
  p->context = context;
  p->control = control;
  p->win = win;

  context_add_callback(context, _context_property_msg_receive, p);

  Evas_Object *frame, *scroller, *bx, *bg;
  bg = elm_bg_add(win);

  scroller = elm_scroller_add(win);
  evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(scroller);
  elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
  p->scroller = scroller;

  //elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

  Evas_Object* pbx = elm_box_add(win);
  evas_object_size_hint_weight_set(pbx, EVAS_HINT_EXPAND, 0);
  evas_object_size_hint_fill_set(pbx, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_align_set(pbx, 0.0, 0.0);
  elm_object_content_set(p->scroller, pbx);
  evas_object_show(pbx);
  p->box_parent = pbx;

  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_align_set(bx, 0.0, 0.0);
  p->box_prefab_info = bx;
  //fill box with stuff
  _prefab_prop_create(p, bx);

  //elm_box_pack_end(pbx, p->box_prefab_info);
  //evas_object_show(bx);

  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_fill_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_align_set(bx, 0.0, 0.0);
  p->box = bx;
  //elm_object_content_set(p->scroller, p->box);
  elm_box_pack_end(pbx, p->box);
  evas_object_show(bx);

  /*
  frame = elm_frame_add(win);
  elm_object_text_set(frame, "Properties");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  evas_object_show(frame);
  p->root = frame;

  elm_object_content_set(frame, scroller);
  */

  elm_object_content_set(bg, scroller);
  p->root = bg;

  return p;
}

static void
_prefab_unlink(
      void *data,
      Evas_Object *obj,
      void *event_info)
{
  Object* o = data;
  object_prefab_unlink(o);
  EINA_LOG_ERR("TODO: unlink in a better way: don't recreate widgets,"
        " just update the components pointer (and data...but should be the same)");
  PropertyView* pw = evas_object_data_get(obj, "propertyview");
  property_object_show(pw, o);
}


void
property_object_show(PropertyView* pw, Object* o)
{
  property_clear_components(pw);
  elm_box_unpack_all(pw->box_parent);

  if (o->prefab.prefab) {
    elm_box_pack_end(pw->box_parent, pw->box_prefab_info);
    evas_object_show(pw->box_prefab_info);
    Evas_Object* en = evas_object_data_get(pw->box_prefab_info, "entry");
    elm_object_text_set(en, o->prefab.name);
    Evas_Object* bt = evas_object_data_get(pw->box_prefab_info, "button");
    evas_object_smart_callback_add(bt, "clicked", _prefab_unlink, o);
  }
  else
  evas_object_hide(pw->box_prefab_info);

  elm_box_pack_end(pw->box_parent, pw->box);
  evas_object_show(pw->box);

  ComponentProperties* cp;
  Component* c;
  Eina_List* l;

  cp = create_component_properties(o->component, pw, false);
  property_add_component(pw, cp);
  cp->component = o->component;
  component_property_update_data(cp);

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    cp = create_component_properties(c, pw, true);
    property_add_component(pw, cp);
    cp->component = c;
    component_property_update_data(cp);
  }
}

void
property_update_component(PropertyView* pw, const char* name)
{
  Eina_List* l;
  ComponentProperties* cp;

  EINA_LIST_FOREACH(pw->component_widgets, l, cp) {
    if (!strcmp(cp->name, name)) {
      component_property_update_data(cp);
    }
  }

}

static void
_property_holder_free_cb(void *data)
{
  PropertyHolder* ph = data;
  eina_list_free(ph->items);
  eina_list_free(ph->objects);
  printf("did you came in the prop holder? \n");
  free(ph);
}

/////////////////////
static Eina_Hash* s_ph = NULL;

void
property_holder_init()
{
  s_ph = eina_hash_pointer_new(_property_holder_free_cb);
}

void
property_holder_object_add(const void* data, const Property* p, Evas_Object* o)
{
  PropertyHolder* ph = eina_hash_find(s_ph, &data);
  if (!ph) {
    ph = calloc(1, sizeof *ph);
    eina_hash_add(s_ph, &data, ph);
  }

  if (!ph->property) ph->property = p;

  ph->objects = eina_list_append(ph->objects, o);
}

void
property_holder_object_del(void* data, Evas_Object* o)
{
  PropertyHolder* ph = eina_hash_find(s_ph, &data);
  if (!ph) return;

  ph->objects = eina_list_remove(ph->objects, o);
}

void
property_holder_del(void* data)
{
  eina_hash_del_by_key(s_ph, &data);
}

void
property_holder_genlist_item_add(const void* data, Elm_Object_Item* i)
{
  PropertyHolder* ph = eina_hash_find(s_ph, &data);
  if (!ph) {
    EINA_LOG_DBG("add data %p", data);
    ph = calloc(1, sizeof *ph);
    eina_hash_add(s_ph, &data, ph);
  }

  EINA_LOG_DBG("add item %p", i);
  ph->items = eina_list_append(ph->items, i);
}

void
property_holder_update(void* data)
{
  PropertyHolder* ph = eina_hash_find(s_ph, &data);
  if (!ph) {
    EINA_LOG_WARN("cannot find data %p", data);
    return;
  }

  Eina_List* l;
  Elm_Object_Item* i;

  EINA_LIST_FOREACH(ph->items, l, i) {
    EINA_LOG_DBG("I update the item %p", i);
    elm_genlist_item_update(i);
  }

  Evas_Object* o;
  EINA_LIST_FOREACH(ph->objects, l, o) {
    EINA_LOG_DBG("I update the object %p", o);

    //int offset = property_offset_get(ph);
    //const char** str = data + offset;
    if (ph->property->type == EET_T_STRING) {
      const char** str = data;
      const char* s = elm_object_text_get(o);
      if (!*str) break;
      if (strcmp(*str,s)) 
      elm_object_text_set(o, *str );
    }
    else if (ph->property->type == EET_T_DOUBLE) {

      double d;
      //int offset = property_offset_get(p);
      //memcpy(&d, (void*)data + offset, sizeof d);
      memcpy(&d, data, sizeof d);
      double old = elm_spinner_value_get(o);
      if (old != d) {
        elm_spinner_value_set(o, d );
      }
    }
    else if (ph->property->type == PROPERTY_OBJECT) {
      ObjectPointer* op = data;
      const char* s = elm_object_text_get(o);
      const char* str;
      if (!op->object) str = "none";
      else str = op->object->name;

      if (strcmp(str,s)) 
      elm_object_text_set(o, str );
    }
    else EINA_LOG_ERR("property not yet supported: %d", ph->property->type);
  }
}

void
property_prefab_show(PropertyView* pw, Prefab* p)
{
  property_clear_components(pw);

  ComponentProperties* cp;
  Component* c;
  Eina_List* l;
  /*

  cp = create_component_properties(o->component, pw, false);
  property_add_component(pw, cp);
  cp->component = o->component;
  component_property_update_data(cp);
  */

  EINA_LIST_FOREACH(p->components, l, c) {
    cp = create_component_properties(c, pw, true);
    property_add_component(pw, cp);
    cp->component = c;
    component_property_update_data(cp);
  }

}

