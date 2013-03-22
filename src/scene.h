#ifndef __scene__
#define __scene__
#include <Eina.h>
#include "object.h"
#include "camera.h"
#include "fbo.h"

typedef struct _Scene Scene;

struct _Scene {
  //list of objects
  Eina_List* objects;
  Eina_List* ortho;
  struct View* view;

  Object* quad_outline;
  Object* quad_color;
  Fbo* fbo_selected;
  Fbo* fbo_all;

  Object* selected; //TODO put in context
};

Scene* create_scene();
void scene_add_object(Scene* s, Object* o);
void scene_add_object_ortho(Scene* s, Object* o);
void scene_draw(Scene* s, Camera* c);
void scene_update(Scene* s);

void scene_destroy(Scene* s);

#endif
