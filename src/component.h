#ifndef __component__
#define __component__
#include <Eina.h>
#include <Elementary.h>

typedef struct _ComponentDesc ComponentDesc;
typedef struct _Component Component;
typedef struct _ComponentManager ComponentManager;

//rename ComponentDefinition?
struct _ComponentDesc {
  const char* name;
  void* (*create)(); 
  Eina_Inarray* (*properties)();
  void (*init)(Component* c); 
  void (*update)(Component* c, double dt); 
};

typedef Component* (*create_component_function)(); 
typedef Eina_List* (*create_components_function)(); 

struct _Component {
  void* data;
  ComponentDesc *funcs;
  Eina_Inarray* properties;
  struct _Object* object;
  const char* name;
};

Component* create_component(ComponentDesc *f); 

struct _ComponentManager {
  Eina_List *components;
  //Eina_Hash *component_widgets;
  Evas_Object* win;
  struct _Control* control;
  void* libhandle;
};

ComponentManager*  create_component_manager(Evas_Object* win, struct _Control* c);
//void component_manager_add(ComponentManager* cm, Component* m);

void component_manager_load(ComponentManager* cm);
void component_manager_unload(ComponentManager* cm);

#endif
