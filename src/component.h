#ifndef __component__
#define __component__
#include <Eina.h>

typedef struct _ComponentFuncs ComponentFuncs;
typedef struct _Component Component;

struct _ComponentFuncs {
  void (*init)(Component* c); 
  void (*update)(Component* c, double dt); 
};

typedef Component* (*create_component_function)(); 

struct _Component {
  void* data;
  ComponentFuncs funcs;
  Eina_Inarray* properties;
  struct _Object* object;
  const char* name;
};

Component* create_component(const char* name, ComponentFuncs f, void* data, Eina_Inarray* properties); 

#endif
