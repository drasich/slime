#include "component.h"
#include "ui/property_view.h"
#include "control.h"
#include <dlfcn.h>

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

  ComponentProperties* cp = create_my_prop(c->name, c->properties, cm->win, cm->control);

  eina_hash_add(
        cm->component_widgets,
        c->name,
        cp);
}

ComponentManager* 
create_component_manager(Evas_Object* win, Control* c)
{
  ComponentManager* cm = calloc(1, sizeof *cm);
  cm->control = c;
  cm->win = win;
  printf("create compo manager\n");

  return cm;
}

void
component_manager_load(ComponentManager* cm)
{
  if (cm->libhandle) return;

  cm->libhandle = dlopen("./build/libgameshared.so", RTLD_NOW);

  if (!cm->libhandle) {
    printf("Error loading DSO: %s\n", dlerror());
  }
  else 
  printf("libsuccess\n");

  //create_component_function  initfunc = dlsym(cm->libhandle, "create_enemy");
  create_components_function  initfunc = dlsym(cm->libhandle, "create_components");

  if (!initfunc) {
    printf("Error loading init function: %s\n", dlerror());
    dlclose(cm->libhandle);
  }
  else 
    printf("symbol success\n");

  Eina_List* l = initfunc();
  
}

void
component_manager_unload(ComponentManager* cm)
{
  dlclose(cm->libhandle);
  cm->libhandle = NULL;
}

