#include "dragger.h"
#include "property.h"
#include "component.h"
#include "object.h"

static void
_dragger_init(Component* component)
{
  //Dragger* d = component->data;
}

static void
_dragger_draw_edit(Component* comp, Matrix4 world, Matrix4 projection)
{
  Dragger* d = comp->data;
  //TODO draw the line
  //TODO remove this function later.
  /*
  Line* l = c->line;
  line_prepare_draw(l, world, projection);
  line_draw(l);
  */
}

ComponentDesc* dragger_desc()
{
  ComponentDesc* cd = calloc(1,sizeof*cd);
  cd->name = "dragger";
  //cd->create = _create_dragger;
  //cd->properties 
  cd->init = _dragger_init,
  cd->draw_edit = _dragger_draw_edit;

  return cd;
};


