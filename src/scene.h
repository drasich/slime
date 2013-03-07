#ifndef __scene__
#define __scene__
#include <Eina.h>
#include "object.h"
#include "camera.h"

typedef struct _Scene Scene;

struct _Scene {
  //list of objects
  Eina_List* objects;
  Eina_List* ortho;
  struct View* view;
  Camera* camera;

  GLuint texture_depth_stencil_id;
  GLuint fbo;
  GLuint rb;
};

Scene* create_scene();
void scene_add_object(Scene* s, Object* o);
void scene_add_object_ortho(Scene* s, Object* o);
void scene_draw(Scene* s);
void scene_update(Scene* s);

void scene_destroy(Scene* s);

#endif
