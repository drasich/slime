#include "component.h"
#include <dlfcn.h>
#include "component/camera.h"
#include "component/meshcomponent.h"
#include "component/armature_component.h"

static int _component_dom = -1;
static Eet_Data_Descriptor* _component_list_descriptor = NULL;

Component*
create_component(ComponentDesc *cd)
{
  EINA_LOG_DOM_WARN(_component_dom, "TODO: calling properties function, to be changed");
  Component* c = calloc(1, sizeof *c);
  c->funcs = cd;
  if (cd->create) c->data = cd->create();
  if (cd->properties) c->properties = cd->properties();
  c->name = cd->name;
  //if (cd->init)
  //cd->init(c);
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

/*
static void
_componentproperties_free_cb(void *data)
{
  ComponentProperties* cp = data;
  free(cp);
  //TODO
}
*/


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
create_component_manager()
{
  _component_dom = eina_log_domain_register("component", EINA_COLOR_CYAN);

  ComponentManager* cm = calloc(1, sizeof *cm);
  return cm;
}

void 
component_manager_load(ComponentManager* cm )
{
  cm->components = eina_list_append(cm->components, component_camera_desc());
  cm->components = eina_list_append(cm->components, component_mesh_desc());
  cm->components = eina_list_append(cm->components, component_armature_desc());

  component_manager_load_name(cm, "./build/libgamelib.so");

  component_descriptor_init(s_component_manager->components);
  //component_list_descriptor();
}

void
component_manager_load_name(ComponentManager* cm, const char* filename )
{
  if (cm->libhandle) {
    EINA_LOG_DOM_INFO(_component_dom, "libhandle is not null, return.");
    return;
  }

  cm->libhandle = dlopen(filename, RTLD_NOW);

  if (!cm->libhandle) {
    EINA_LOG_DOM_ERR(_component_dom, "Error loading DSO: %s", dlerror());
    return;
  }
  else 
  EINA_LOG_DOM_INFO(_component_dom, "Game Component Library successfully loaded");

  //create_component_function  initfunc = dlsym(cm->libhandle, "create_enemy");
  create_components_function  initfunc = dlsym(cm->libhandle, "create_components");

  if (!initfunc) {
    EINA_LOG_DOM_ERR(_component_dom, "Error loading init function: %s", dlerror());
    dlclose(cm->libhandle);
    return;
  }
  else 
  EINA_LOG_DOM_INFO(_component_dom, "Game Component Library, symbols successfully loaded");

  Eina_List* user_components = initfunc();
  cm->components = eina_list_merge(cm->components, user_components);

}

static Eet_Data_Descriptor *_variant_unified_descriptor;

static void
_component_descriptor_delete(ComponentManager* cm)
{
  prefab_property_free();

  free(_component_list_descriptor);
  _component_list_descriptor = NULL;

  free(_variant_unified_descriptor);
  _variant_unified_descriptor = NULL;

  free(component_descriptor);
  component_descriptor = NULL;

}


void
component_manager_unload(ComponentManager* cm)
{
  EINA_LOG_DOM_DBG(_component_dom, "unload");
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
  EINA_LOG_DOM_WARN(_component_dom, "review this function");
  void** data  = (void*)(c->data + p->offset);
  return *data;
}

void
component_property_data_set(Component* c, Property* p, const void* data)
{
  EINA_LOG_DOM_WARN(_component_dom, "review this function");
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

//#undef EINA_LOG_DOMAIN_DEFAULT
//#define EINA_LOG_DOMAIN_DEFAULT _component_dom

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
    EINA_LOG_DOM_INFO(_component_dom, "Component description : %s", cd->name);
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

  EINA_LOG_DOM_ERR(_component_dom, "Could not find component description %s", name);
  return NULL;

}

void
component_manager_destroy(ComponentManager* cm)
{
  free(cm);
}

void
components_del(Eina_List* components)
{
  Component* c;
  EINA_LIST_FREE(components, c) {
    if (c->funcs->del)
    c->funcs->del(c);
  }
}

Eet_Data_Descriptor*
component_list_descriptor()
{
  Eet_Data_Descriptor* edd = _component_list_descriptor;
  if (edd) return edd;

  printf("component list descriptor going to create!\n");

  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, ComponentList);
  edd = eet_data_descriptor_stream_new(&eddc);

  EET_DATA_DESCRIPTOR_ADD_LIST(
    edd, ComponentList, "list", list,
    component_descriptor);


  _component_list_descriptor = edd;

  return edd;
}



ComponentList*
components_copy(Eina_List* components)
{
  ComponentList cl = {.list = components};

  int size;
  void *encoded = eet_data_descriptor_encode(
        component_list_descriptor(),
        &cl,
        &size);

  ComponentList* clnew = eet_data_descriptor_decode(
        component_list_descriptor(),
        encoded,
        size);

  return clnew;
}

