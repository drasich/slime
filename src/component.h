#ifndef __component__
#define __component__
#include <Eina.h>

typedef struct _ComponentFuncs ComponentFuncs;
typedef struct _Component Component;

struct _ComponentFuncs {
  void (*init)(Component* c); 
  void (*update)(Component* c, double dt); 
};

struct _Component {
  void* data;
  ComponentFuncs funcs;
  Eina_Inarray* properties;
  struct _Object* object;
};

Component* create_component(ComponentFuncs f, void* data, Eina_Inarray* properties); 

#endif
