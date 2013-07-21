#ifndef __component__
#define __component__

typedef struct _ComponentFuncs ComponentFuncs;

struct _ComponentFuncs {
  void (*init)(void); 
  void (*update)(double dt); 
};

typedef struct _Component Component;

struct _Component {
  ComponentFuncs funcs;
  Eina_Inarray* properties;
};

#endif
