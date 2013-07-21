#include "component.h"

Component*
create_component(ComponentFuncs f, void* data, Eina_Inarray* properties)
{
  Component* c = calloc(1, sizeof *c);
  c->funcs = f;
  c->data = data;
  c->properties = properties;
}

