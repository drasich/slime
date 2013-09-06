#include "scene.h"
#include "gl.h"

Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  s->ortho = NULL;
  eina_init();

  return s;
}

void
scene_add_object(Scene* s, Object* o)
{
  s->objects = eina_list_append(s->objects, o);
  o->scene = s;
}

void
scene_remove_object(Scene* s, Object* o)
{
  s->objects = eina_list_remove(s->objects, o);
  o->scene = 0;
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

void
scene_camera_set(Scene* s, Object* camera)
{
  s->camera = camera;
}

Object*
scene_object_get(Scene* s, const char* name)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    if (!strcmp(o->name, name))
          return o;
  }
  return NULL;
}



static Eet_Data_Descriptor *_scene_descriptor;

static void
_scene_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Scene);
  _scene_descriptor = eet_data_descriptor_stream_new(&eddc);

  EET_DATA_DESCRIPTOR_ADD_LIST
   (_scene_descriptor, Scene, "objects", objects,
    object_descriptor);

}


static const char SCENE_FILE_ENTRY[] = "scene";

Eina_Bool
scene_write(const Scene* s)
{
  object_descriptor_init();
  _scene_descriptor_init();

  const char* filename = "scene.eet";

  Eina_Bool ret;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_WRITE);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, _scene_descriptor, SCENE_FILE_ENTRY, s, EINA_TRUE);
  eet_close(ef);
  if (ret) {
    printf("return value for save looks ok \n");
  }
  else
    printf("return value for save NOT OK \n");

  return ret;


}

void output(void *data, const char *string)
{
  printf("%s", string);
}


Scene*
scene_read()
{
  Scene* s;
  const char* filename = "scene.eet";

  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return NULL;
  }

  s = eet_data_read(ef, _scene_descriptor, SCENE_FILE_ENTRY);
  eet_close(ef);
  printf("data dump\n");
  eet_data_dump(ef, SCENE_FILE_ENTRY, output, NULL);
  printf("data dump end\n");
 
  return s;
  
}

void
scene_print(Scene* s)
{
  printf("scene print\n");

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
   printf("  object name : %s \n", o->name);

  printf("scene print end\n");
}

