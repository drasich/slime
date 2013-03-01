#include "scene.h"
#include "gl.h"

Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  eina_init();
  s->camera = create_camera();
  s->camera->object.name = "camera";
  //Vec3 v = {10,10,10};
  Vec3 v = {0,0,20};
  //Vec3 axis = {0,1,0};
  //Quat q = quat_angle_axis(3.14f/4.0f, axis);
  s->camera->object.Position = v;
  Vec3 at = {0,0,0};
  Vec3 up = {0,1,0};
  s->camera->object.Orientation = quat_lookat(v, at, up);
  //mat4_set_perspective(s->camera->projection, M_PI/4.0, 1.6 ,1,1000.0f);

  return s;
}

void
scene_add_object(Scene* s, Object* o)
{
  s->objects = eina_list_append(s->objects, o);
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

  //TODO destroy camera

  free(s);
  eina_shutdown();
}

void
scene_draw(Scene* s, int w, int h)
{
  Matrix4 mt, mr, cam_mat, mo;

  mat4_set_translation(mt, s->camera->object.Position);
  mat4_set_rotation_quat(mr, s->camera->object.Orientation);
  //Quat q = s->camera->object.Orientation;
  //printf("camera rot %f, %f, %f, %f \n", q.X, q.Y, q.Z, q.W);
  mat4_multiply(mt, mr, cam_mat);
  mat4_inverse(cam_mat, cam_mat);

  //TODO get values from camera
  float aspect = (float)w/(float)h;
  Matrix4 projection;
  //mat4_set_frustum(projection, -aspect*1,aspect*1,-1,1,1,1000.0f);
  mat4_set_perspective(projection, M_PI/4.0, aspect ,1.0f,1000.0f);

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat, mo, mo);
    object_draw(o, mo, projection);
  }
}

void
scene_update(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_update(o);
}

