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
  printf("TODO clean components here\n");
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
    v = vec3_add(v, o->Position);
    last = o;
    i++;
  }

  int nb = eina_list_count(objects);
  if (nb == 1) {
    property_update_components(pw, last);
  }
  else if (nb > 1) {
    property_set(pw, NULL);
    //ComponentProperties* cp = eina_hash_find(pw->component_widgets_backup,"multiple");
    //cp->data = &pw->context->mos;
    //_property_update_data(pw->current, &pw->context->mos);
  }
}

//TODO change this function to something like update_component_transform 
//and remove current
void
property_update2(PropertyView* pw, Object* o)
{
  if ( pw->current->data == o) {
    component_property_update_data(pw->current, o);
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
  if (pw->current) {
    //elm_object_content_unset(pw->scroller);
    evas_object_hide(pw->current->box);
    elm_box_unpack_all(pw->box);
  }

  if (cp) {
    //elm_object_content_set(pw->scroller, cp->box);
    //evas_object_show(cp->box);
    elm_box_pack_end(pw->box, cp->box);
    evas_object_show(cp->box);
  }

  pw->current = cp;
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


static void
_property_entry_free_cb(void *data)
{
   //free(data);
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

  /*
  frame = elm_frame_add(win);
  elm_object_text_set(frame, "Properties");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_fill_set(frame, EVAS_HINT_FILL, 0.0);

  evas_object_show(frame);
  p->root = frame;

  elm_object_content_set(frame, scroller);
  */
  p->root = scroller;

  p->component_widgets_backup = eina_hash_string_superfast_new(_property_entry_free_cb);

  //TODO remove these arrays, and the ComponentProperties from propertyview
  p->arr = _object_init_array_properties();
  p->array_multiple_objects = _multiple_objects_init_array_properties();
  ComponentProperties* transform = create_my_prop("transform", p->arr, win, control, false);

  eina_hash_add(
        p->component_widgets_backup,
        transform->name,
        transform);
  
  ComponentProperties* manyobj = create_my_prop("multiple", p->array_multiple_objects, win, control, false);
  manyobj->callback = _changed_multiple_object;

  eina_hash_add(
        p->component_widgets_backup,
        manyobj->name,
        transform);

  return p;
}


void
property_update_components(PropertyView* pw, Object* o)
{
  property_clear_components(pw);

  ComponentProperties* cp = eina_hash_find(pw->component_widgets_backup,"transform");
  property_add_component(pw, cp);
  cp->data = o;
  pw->current = cp;
  component_property_update_data(cp, o);
  
  Eina_List* l;
  Component* c;
  EINA_LIST_FOREACH(o->components, l, c) {
    cp = create_component_properties(c, pw);
    eina_hash_add(
          pw->component_widgets_backup,
          c->name,
          cp);
    property_add_component(pw, cp);
    cp->data = c->data;
  }

}


