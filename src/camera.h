#ifndef __camera__
#define __camera__
#include "object.h"
#include "matrix.h"
#include "intersect.h" //TODO remove
#include "geometry.h"
#include "component/camera.h"


typedef struct _ViewCamera ViewCamera;
struct _ViewCamera
{
  Object* object;
  Camera* camera_component;
};

ViewCamera* view_camera_new();
ViewCamera* create_camera_from_object(Object* o);

void camera_pan(ViewCamera* cam, Vec3 v); // same as trans
void camera_rotate(ViewCamera* cam, Quat q); // same as -pos, rot, + pos
void camera_rotate_around_position(ViewCamera* cam, Quat q, Vec3 v);  //same as rotate the cam pos and look at v
void camera_update_projection(ViewCamera* c);
void camera_update_orthographic(ViewCamera* c);

void camera_set_resolution(ViewCamera* cam, int w, int h);

void camera_lookat(ViewCamera* c, Vec3 at);

void camera_rotate_around(ViewCamera* c, Quat q, Vec3 pivot);

void camera_pan(ViewCamera* c, Vec3 t);

void camera_recalculate_origin(ViewCamera* c);

Ray ray_from_screen(ViewCamera* c, double x, double y, float length);
Vec2 camera_world_to_screen(ViewCamera* vc, Vec3 world);

void camera_get_frustum(ViewCamera* c, Frustum* out);

void camera_get_frustum_planes(ViewCamera* c, Plane* p);
void camera_get_frustum_planes_rect(
      ViewCamera*c,
      Plane* out_planes,
      float left, float top, float width, float height);


#endif
