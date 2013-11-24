#include "scene.h"
#include "gl.h"

Scene*
scene_new()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  s->name = "empty";
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
scene_del(Scene* s)
{
  Eina_List *l;
  Object *o;

  EINA_LIST_FREE(s->objects, o) {
    object_destroy(o);
  }

  free(s);
}

/*
void
scene_draw(Scene* s, Camera* c)
{

}
*/

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
  s->camera_name = camera->name;
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
static Property* _object_ps;

void
scene_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Scene);
  _scene_descriptor = eet_data_descriptor_stream_new(&eddc);

  EET_DATA_DESCRIPTOR_ADD_BASIC(
        _scene_descriptor,
        Scene, "camera", camera_name, EET_T_STRING);

  _object_ps = property_set_object();

  EET_DATA_DESCRIPTOR_ADD_LIST
   (_scene_descriptor, Scene, "objects", objects,
    //object_descriptor);
    _object_ps->descriptor);
}

void 
scene_descriptor_delete(void)
{
  free(_scene_descriptor);
  _scene_descriptor = NULL;
  object_descriptor_delete();
  _object_ps = NULL;
}


static const char SCENE_FILE_ENTRY[] = "scene";

Eina_Bool
scene_write(const Scene* s, const char* filename)
{
  printf("going to print scene %s \n", s->name);
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

static void 
_output(void *data, const char *string)
{
  printf("%s", string);
}


Scene*
scene_read(const char* filename)
{
  printf("scene read start\n");
  Scene* s;

  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return NULL;
  }

  s = eet_data_read(ef, _scene_descriptor, SCENE_FILE_ENTRY);
  printf("scene read data dump\n");
  eet_data_dump(ef, SCENE_FILE_ENTRY, _output, NULL);
  printf("scene read data dump end\n");
  eet_close(ef);
 
  return s;  
}

void
scene_post_read(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    if (s->camera_name && !strcmp(o->name, s->camera_name)) {
      printf("found camera!!!! %s\n", s->camera_name);
      s->camera = o;
    }
    object_post_read(o);
  }

}

void
scene_init(Scene* s)
{
  scene_post_read(s);
}

#include "component/meshcomponent.h"

void
scene_print(Scene* s)
{
  printf("scene print\n");

  Eina_List *cl;
  Eina_List *l;
  Object *o;
  Component* c;
  EINA_LIST_FOREACH(s->objects, l, o) {
   printf("  object name : %s \n", o->name);
   EINA_LIST_FOREACH(o->components, cl, c) {
     printf("     component name, pointer : %s, %p \n", c->name,c);
     if (!c->name) continue;
     if (!strcmp(c->name, "mesh")) {
       MeshComponent* mc = c->data;
       if (mc->shader_handle.name)
       printf("        mesh handle name, mc pointer : %s, %p \n", mc->mesh_handle.name, mc);
       else
       printf("SHADER HANDLE NAME IS NULL\n");
       if (mc->shader_handle.name)
       printf("        shader handle name, mc pointer : %s, %p \n", mc->shader_handle.name, mc);
       else
       printf("SHADER HANDLE NAME IS NULL\n");
       if (mc->shader_instance) {
         shader_instance_print(mc->shader_instance);

       }
     }
     /*
     else if (!strcmp(c->name, "camera")) {
       Camera* cc = c->data;
       printf("       camera width : %f \n", cc->width);
     }
     else if (!strcmp(c->name, "player")) {
       const char** yep = c->data;
       printf("       player name : %s \n", *yep);

     }
     */
   }
  }

  printf("scene print end\n");
}


static Property* s_ps_scene = NULL;

Property*
property_set_scene()
{
  if (s_ps_scene) return s_ps_scene;

  s_ps_scene = property_set_new();
  Property* ps = s_ps_scene;
  ps->name = "scene";
  PROPERTY_SET_TYPE(ps, Scene);


  PROPERTY_BASIC_ADD(ps, Scene, name, EET_T_STRING);

  return ps;
}

Scene*
scene_copy(const Scene* so, const char* name)
{
  Scene* s = calloc(1, sizeof *s);
  memcpy(s, so, sizeof *s);
  s->name = name;

  s->objects = NULL;

  Eina_List* l;
  Object* oo;
  EINA_LIST_FOREACH(so->objects, l, oo) {
    //todo copy objects
    Object* o = object_copy(oo);
    s->objects = eina_list_append(s->objects, o);
  }

  //s->camera  //find the camera

  return s;
}
