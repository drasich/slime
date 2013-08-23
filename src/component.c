#include "component.h"
#include "ui/property_view.h"
#include "control.h"
#include <dlfcn.h>
#include "component/camera.h"

Component*
create_component(ComponentDesc *cd)
{
  printf("TODO creating component : calling properties function, to be changed\n");
  Component* c = calloc(1, sizeof *c);
  c->funcs = cd;
  if (cd->create) c->data = cd->create();
  c->properties = cd->properties();
  c->name = cd->name;
  return c;
}

/*
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
*/

static void
_componentproperties_free_cb(void *data)
{
  ComponentProperties* cp = data;
  free(cp);
  //TODO
}


/*
static void
_create_widgets(ComponentManager* cm)
{
  cm->component_widgets = eina_hash_string_superfast_new(_componentproperties_free_cb);

  Eina_List* l;
  ComponentDesc* c;

  printf("TODO creating widgets : calling properties function, to be changed\n");

  EINA_LIST_FOREACH(cm->components, l, c) {
    ComponentProperties* cp = create_my_prop(c->name, c->properties(), cm->win, cm->control);

    eina_hash_add(
          cm->component_widgets,
          c->name,
          cp);
  }
}
*/


ComponentManager* 
create_component_manager(Evas_Object* win, Control* c)
{
  ComponentManager* cm = calloc(1, sizeof *cm);
  cm->control = c;
  //cm->win = win;
  printf("create compo manager\n");
  cm->components = eina_list_append(cm->components, &camera_desc);


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

  Eina_List* user_components = initfunc();
  cm->components = eina_list_merge(cm->components, user_components);
  printf("init func done\n");
  //_create_widgets(cm);
  printf("create components end\n");
  
}

void
component_manager_unload(ComponentManager* cm)
{
  dlclose(cm->libhandle);
  cm->libhandle = NULL;

  //eina_hash_free(cm->component_widgets);
  //cm->component_widgets = NULL;

  eina_list_free(cm->components);
  cm->components = NULL;
}

