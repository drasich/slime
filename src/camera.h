#ifndef __camera__
#define __camera__
#include "object.h"
#include "matrix.h"

typedef struct _Camera Camera;
struct _Camera
{
  struct _Object object;
  Matrix4 projection;
  //other things like fov etc..

};

Camera* create_camera();

void camera_pan(Camera* cam, Vec3 v); // same as trans
void camera_rotate(Camera* cam, Quat q); // same as -pos, rot, + pos
void camera_rotate_around_position(Camera* cam, Quat q, Vec3 v);  //same as rotate the cam pos and look at v

#endif
