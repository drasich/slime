#ifndef __camera__
#define __camera__
#include "object.h"
#include "matrix.h"
#include "intersect.h" //TODO remove
#include "geometry.h"
#include "component/camera.h"


typedef struct _Camera Camera;
struct _Camera
{
  Object* object;
  CCamera* camera_component;
};

Camera* create_camera();
Camera* create_camera_from_object(Object* o);

void camera_pan(Camera* cam, Vec3 v); // same as trans
void camera_rotate(Camera* cam, Quat q); // same as -pos, rot, + pos
void camera_rotate_around_position(Camera* cam, Quat q, Vec3 v);  //same as rotate the cam pos and look at v
void camera_update_projection(Camera* c);
void camera_update_orthographic(Camera* c);

void camera_set_resolution(Camera* cam, int w, int h);

void camera_lookat(Camera* c, Vec3 at);

void camera_rotate_around(Camera* c, Quat q, Vec3 pivot);

void camera_pan(Camera* c, Vec3 t);

void camera_recalculate_origin(Camera* c);

Ray ray_from_screen(Camera* c, double x, double y, float length);

void camera_get_frustum(Camera* c, Frustum* out);

void camera_get_frustum_planes(Camera* c, Plane* p);
void camera_get_frustum_planes_rect(
      Camera*c,
      Plane* out_planes,
      float left, float top, float width, float height);


#endif
