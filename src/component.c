#include "component.h"
#include "ui/property_view.h"

Component*
create_component(const char* name, ComponentFuncs f, void* data, Eina_Inarray* properties)
{
  Component* c = calloc(1, sizeof *c);
  c->funcs = f;
  c->data = data;
  c->properties = properties;
  c->name = name;
  return c;
}

void
component_manager_add(ComponentManager* cm, Component* c)
{
  cm->components = eina_list_append(cm->components, c);

  Evas_Object* win;
  Control* control;

  ComponentProperties* mp = create_my_prop(c->name, c->properties, win, control);

  eina_hash_add(
        cm->component_widgets,
        c->name,
        mp);

}
