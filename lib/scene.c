#include "scene.h"
#include "gl.h"
#include "log.h"
#include "prefab.h"

Scene*
scene_new()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  s->name = "empty";
  s->clear_color = vec4(0.2,0.2,0.2,1);
  return s;
}


void
scene_add_object(Scene* s, Object* o)
{
  s->objects = eina_list_append(s->objects, o);
  o->scene = s;
  o->id = ++s->last_id;
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
    object_del(o);
  }

  printf("going to delete scene\n");

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
  s->camerapointer.id = camera->id;
  s->camerapointer.object = camera;
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



//static Property* _object_ps;
static Property* s_ps_scene = NULL;

//TODO remove/handle this function
void
scene_descriptor_init(void)
{
  property_set_scene();
}

void 
scene_descriptor_delete(void)
{
  //fix this function
  object_descriptor_delete();
  //_object_ps = NULL;


  free(s_ps_scene->descriptor);
  free(s_ps_scene);
  s_ps_scene = NULL;
}


static const char SCENE_FILE_ENTRY[] = "scene";

Eina_Bool
scene_write(const Scene* s, const char* filename)
{
  Eina_Bool ret;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_WRITE);
  if (!ef) {
    EINA_LOG_DOM_ERR(log_scene_dom, "error reading file %s", filename);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, s_ps_scene->descriptor, SCENE_FILE_ENTRY, s, EINA_TRUE);
  eet_close(ef);

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
  Scene* s;

  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    EINA_LOG_DOM_ERR(log_scene_dom, "error reading file '%s'.", filename);
    return NULL;
  }

  s = eet_data_read(ef, s_ps_scene->descriptor, SCENE_FILE_ENTRY);
  //printf("scene read data dump\n");
  //eet_data_dump(ef, SCENE_FILE_ENTRY, _output, NULL);
  //printf("scene read data dump end\n");
  eet_close(ef);
 
  return s;  
}

static void
_scene_object_post_read(Scene* s, Object* o)
{
    if (s->camerapointer.id == o->id)
    s->camerapointer.object = o;

    o->scene = s;
    if (o->id > s->last_id)
    s->last_id = o->id;

    Eina_List* l;
    Object* child;
    EINA_LIST_FOREACH(o->children, l, child) {
      _scene_object_post_read(s, child);
    }
}


void
scene_post_read(Scene* s)
{
  EINA_LOG_DOM_DBG(log_scene_dom, "read start scene name is %s\nobjects number is %d", s->name, eina_list_count(s->objects));
  s->last_id = 0;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    _scene_object_post_read(s, o);

    object_post_read(o);
  }

  EINA_LOG_DOM_DBG(log_scene_dom, "scene name is %s\nobjects number is %d", s->name, eina_list_count(s->objects));

}

void
scene_init(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    if (s->camerapointer.id == o->id)
    s->camerapointer.object = o;

    if (o->prefab.prefab) {
      ComponentList* cl = components_copy(o->prefab.prefab->components);
      o->components = cl->list;
      o->prefab.name = "";
      object_post_read(o);
      o->prefab.prefab = NULL;
      o->prefab.name = "";
      free(cl);
    }
  }
}

#include "component/meshcomponent.h"
#include "component/armature_component.h"

void
scene_print(Scene* s)
{
  printf("scene print\n");

  Eina_List *cl;
  Eina_List *l;
  Object *o;
  Component* c;
  EINA_LIST_FOREACH(s->objects, l, o) {
   printf("  object name, id : %s, %llu \n", o->name, o->id);
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
     else if (!strcmp(c->name, "armature")) {
       ArmatureComponent* ac = c->data;
       if (ac->armature_handle.name)
       printf("        ARMATURE handle name, ac pointer : %s, %p \n", ac->armature_handle.name, ac);

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



Property*
property_set_scene()
{
  if (s_ps_scene) return s_ps_scene;

  s_ps_scene = property_set_new();
  Property* ps = s_ps_scene;
  ps->name = "scene";
  PROPERTY_SET_TYPE(ps, Scene);

  PROPERTY_BASIC_ADD(ps, Scene, name, EET_T_STRING);

  Property *obp = property_set_object_pointer("camera");
  PROPERTY_SUB_NESTED_ADD(ps, Scene, camerapointer, obp);

  Property* vec4 = property_set_vec4();
  PROPERTY_SUB_NESTED_ADD(ps, Scene, clear_color, vec4);

  Property* object_ps = property_set_object();

  EET_DATA_DESCRIPTOR_ADD_LIST(
        ps->descriptor, Scene, "objects", objects,
    //object_descriptor);
    object_ps->descriptor);

  return ps;
}

Scene*
scene_copy(const Scene* so, const char* name)
{
  /*
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
  */

  int size;
  void *encoded = eet_data_descriptor_encode(
        s_ps_scene->descriptor,
        so,
        &size);

  Scene* s = eet_data_descriptor_decode(
        s_ps_scene->descriptor,
        encoded,
        size);

  s->name = name;

  return s;
}

static void
_object_id_gen(Scene* s, Object* o)
{
  o->id = ++ s->last_id;

  Eina_List* l;
  Object* child;
  EINA_LIST_FOREACH(o->children, l, child){
    _object_id_gen(s, child);
  }

}

void
scene_objects_id_generate(Scene* s)
{
  s->last_id = 0;

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    _object_id_gen(s, o);
  }
  
}

void
scene_reference_get(Scene* s, ObjectPointer* op)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    if (op->id == o->id) {
      op->object = o;
      return;
    }
  }
}

