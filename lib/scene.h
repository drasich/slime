#ifndef __scene__
#define __scene__
#include <Eina.h>
#include "object_pointer.h"
#include "object.h"
//#include "camera.h"
#include "fbo.h"

typedef struct _Scene Scene;

struct _Scene {
  Eina_List* objects;
  const char* name;
  unsigned long long last_id;
  ObjectPointer camerapointer;
  Vec4 clear_color;
};

Scene* scene_new();
void scene_add_object(Scene* s, Object* o);
void scene_remove_object(Scene* s, Object* o);
//void scene_draw(Scene* s, Camera* c);
void scene_update(Scene* s);

void scene_del(Scene* s);

void scene_camera_set(Scene* s, Object* camera);

Object* scene_object_get(Scene* s, const char* name);

Eina_Bool scene_write(const Scene* s, const char* filename);
Scene* scene_read(const char* filename);
void scene_print(Scene* s);

void scene_post_read(Scene* s);
void scene_descriptor_init(void);
void scene_descriptor_delete(void);
void scene_init(Scene* s);

Property* property_set_scene();

Scene* scene_copy(const Scene* s, const char* name);

void scene_objects_id_generate(Scene* s);
void scene_reference_get(Scene* s, ObjectPointer* op);

#endif
