#include "scene.h"
#include "gl.h"

static void
populate_scene(Scene* s)
{
  //Object* o = create_object_file("model/smallchar.bin");
  Object* o = create_object_file("model/cube.bin");
  o->name = "cube";
  //Object* o = create_object_file("model/simpleplane.bin");
  //TODO free shader
  Shader* shader_simple = create_shader("shader/simple.vert", "shader/simple.frag");
  o->mesh->shader = shader_simple;

  Vec3 t = {0,0,0};
  //Vec3 t = {0,0,0};
  object_set_position(o, t);
  scene_add_object(s,o);

  //animation_play(o, "walkquat", LOOP);

  Object* yep = create_object_file("model/smallchar.bin");
  //animation_play(yep, "walkquat", LOOP);
  yep->mesh->shader = shader_simple;

  yep->name = "2222222";
  Vec3 t2 = {-10,0,0};
  object_set_position(yep, t2);
  scene_add_object(s,yep);

  //GLint bits;
  //gl->glGetIntegerv(GL_DEPTH_BITS, &bits);
  //printf("depth buffer %d\n\n", bits);
}


Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  s->ortho = NULL;
  eina_init();
  populate_scene(s);

  return s;
}

void
scene_add_object(Scene* s, Object* o)
{
  s->objects = eina_list_append(s->objects, o);
  o->scene = s;
}

void
scene_add_object_ortho(Scene* s, Object* o)
{
  s->ortho = eina_list_append(s->ortho, o);
  o->scene = s;
}



void
scene_destroy(Scene* s)
{
  Eina_List *l;
  Object *o;

  EINA_LIST_FREE(s->objects, o) {
    object_destroy(o);
  }

  EINA_LIST_FREE(s->ortho, o) {
    object_destroy(o);
  }

  free(s);
  eina_shutdown();
}

void
scene_draw(Scene* s, Camera* c)
{

}

void
scene_update(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_update(o);
}

