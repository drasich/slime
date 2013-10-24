#include "component.h"
#include "ui/property_view.h"
#include "control.h"
#include <dlfcn.h>
#include "component/camera.h"
#include "component/meshcomponent.h"

Component*
create_component(ComponentDesc *cd)
{
  printf("TODO creating component : calling properties function, to be changed\n");
  Component* c = calloc(1, sizeof *c);
  c->funcs = cd;
  if (cd->create) c->data = cd->create();
  if (cd->properties) c->properties = cd->properties();
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

  cm->components = eina_list_append(cm->components, &camera_desc);
  cm->components = eina_list_append(cm->components, &mesh_desc);

  Eina_List* user_components = initfunc();
  cm->components = eina_list_merge(cm->components, user_components);
  printf("init func done\n");
  //_create_widgets(cm);

  component_descriptor_init(s_component_manager->components);
  printf("create components end\n");

  
}

static Eet_Data_Descriptor *_variant_unified_descriptor;

static void
_component_descriptor_delete(ComponentManager* cm)
{
  free(_variant_unified_descriptor);
  _variant_unified_descriptor = NULL;
  free(component_descriptor);
  component_descriptor = NULL;
}


void
component_manager_unload(ComponentManager* cm)
{
  printf("unload\n");
  dlclose(cm->libhandle);
  cm->libhandle = NULL;

  //eina_hash_free(cm->component_widgets);
  //cm->component_widgets = NULL;

  eina_list_free(cm->components);
  cm->components = NULL;

  _component_descriptor_delete(cm);
}


void* 
component_property_data_get(Component* c, Property* p)
{
  printf("warning -- review this function %s\n", __FUNCTION__);
  void** data  = (void*)(c->data + p->offset);
  return *data;
}

void
component_property_data_set(Component* c, Property* p, const void* data)
{
  printf("warning -- review this function %s\n", __FUNCTION__);
  void* cd = c->data;
  memcpy(cd + p->offset, data, p->size);
}


static const char *
_component_type_get(
      const void *data,
      Eina_Bool  *unknow)
{
  const char **name = (const char**) data;
  if (!strcmp(*name, "object")) {
    *unknow = EINA_TRUE;
    return NULL;
  }

  *unknow = EINA_FALSE;

  return *name;
}

static Eina_Bool
_component_type_set(
      const char *type,
      void       *data,
      Eina_Bool   unknow)
{
  const char **name = data;
  *name = type;

  if (!strcmp(*name, "object"))
  return EINA_FALSE;

  return EINA_TRUE;
}


void
component_descriptor_init(Eina_List* component_desc)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Component);
  component_descriptor = eet_data_descriptor_stream_new(&eddc);

  eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
  eddc.func.type_get = _component_type_get;
  eddc.func.type_set = _component_type_set;
  _variant_unified_descriptor = eet_data_descriptor_stream_new(&eddc);


  Eina_List* l;
  ComponentDesc* cd;
  EINA_LIST_FOREACH(component_desc, l, cd) {
    printf("DESCRIPTOR component name : %s\n", cd->name);
    EET_DATA_DESCRIPTOR_ADD_MAPPING(
          _variant_unified_descriptor, cd->name, cd->properties()->descriptor);
    //TODO free cd->properties
  }

  EET_DATA_DESCRIPTOR_ADD_VARIANT(
        component_descriptor, Component, "data", data, name,
        _variant_unified_descriptor);

}

const ComponentDesc*
component_manager_desc_get(const ComponentManager* cm, const char* name)
{
  Eina_List* l;
  ComponentDesc* cd;
  EINA_LIST_FOREACH(cm->components, l, cd) {
    if (!strcmp(cd->name, name)) {
      return cd;
    }
  }

  printf("Could not find component description %s\n", name);
  return NULL;

}
