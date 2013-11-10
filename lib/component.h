#ifndef __component__
#define __component__
#include <Eina.h>
#include <Elementary.h>
#include "matrix.h"
#include "property.h"

typedef struct _ComponentDesc ComponentDesc;
typedef struct _Component Component;
typedef struct _ComponentManager ComponentManager;

struct _Camera;

struct _ComponentDesc {
  const char* name;
  void* (*create)(); 
  Property* (*properties)();
  void (*init)(Component* c); 
  void (*update)(Component* c, double dt); 
  void (*draw)(Component* c, Matrix4 world, const Matrix4 projection); 
  void (*on_property_changed)(Component* c); 
  void (*draw_edit)(Component* c, Matrix4 world, const Matrix4 projection); 
};

typedef Component* (*create_component_function)(); 
typedef Eina_List* (*create_components_function)(); 

struct _Component {
  void* data;
  const ComponentDesc *funcs;
  const Property* properties;
  struct _Object* object;
  const char* name;
  bool disabled;
};

Component* create_component(ComponentDesc *f); 

struct _ComponentManager {
  Eina_List *components;
  void* libhandle;
  Eina_Hash *property_set;
};

ComponentManager*  create_component_manager();
//void component_manager_add(ComponentManager* cm, Component* m);

void component_manager_load(ComponentManager* cm);
void component_manager_unload(ComponentManager* cm);

void* component_property_data_get(Component* c, Property* p);
void component_property_data_set(Component* c, Property* p, const void* data);
Eet_Data_Descriptor* component_descriptor;
void component_descriptor_init(Eina_List* component_desc);

const ComponentDesc* component_manager_desc_get(const ComponentManager* cm, const char* name);
ComponentManager* s_component_manager;

#endif
