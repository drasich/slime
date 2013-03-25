#ifndef __camera__
#define __camera__
#include "object.h"
#include "matrix.h"
#include "intersect.h" //TODO remove

typedef struct _Camera Camera;
struct _Camera
{
  struct _Object object;
  Matrix4 projection;
  Matrix4 orthographic;
  //other things like fov etc..
  float fovy;
  float near;
  float far;
  float aspect;
  int width;
  int height;
  double yaw, pitch, roll;

  Vec3 origin;
  Vec3 local_offset;
};

Camera* create_camera();

void camera_pan(Camera* cam, Vec3 v); // same as trans
void camera_rotate(Camera* cam, Quat q); // same as -pos, rot, + pos
void camera_rotate_around_position(Camera* cam, Quat q, Vec3 v);  //same as rotate the cam pos and look at v
void camera_update_projection(Camera* c);
void camera_update_orthographic(Camera* c);

void camera_set_resolution(Camera* cam, int w, int h);

void camera_lookat(Camera* c, Vec3 at);

void camera_rotate_around(Camera* c, Quat q, Vec3 pivot);

void camera_pan(Camera* c, Vec3 t);
Ray ray_from_screen(Camera* c, double x, double y, float length);

#endif
