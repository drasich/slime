#include "geometry.h"

void frustum_set(
      Frustum* f, 
      float near, 
      float far, 
      Vec3 start, 
      Vec3 direction,
      Vec3 up,
      float fovy,
      float aspect)
{
  f->near = near;
  f->far = far;
  //f->direction = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  f->direction = direction;
  f->start = start;
  //f->up = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));
  f->up = up;
  f->fovy = fovy;
  f->aspect = aspect;
}
