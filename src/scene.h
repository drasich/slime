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
  struct _View* view;
  Object* camera;
};

Scene* create_scene();
void scene_add_object(Scene* s, Object* o);
void scene_remove_object(Scene* s, Object* o);
void scene_add_object_ortho(Scene* s, Object* o);
void scene_draw(Scene* s, Camera* c);
void scene_update(Scene* s);

void scene_destroy(Scene* s);

void scene_camera_set(Scene* s, Object* camera);

Object* scene_object_get(Scene* s, const char* name);

Eina_Bool scene_write(const Scene* s);
Scene* scene_read();
void scene_print(Scene* s);

#endif
