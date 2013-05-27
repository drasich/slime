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

void
aabox_to_obox(AABox a, OBox o, Quat q)
{
  Vec3 x = quat_rotate_vec3(q, vec3(1,0,0));
  Vec3 y = quat_rotate_vec3(q, vec3(0,1,0));
  Vec3 z = quat_rotate_vec3(q, vec3(0,0,1));

  //Vec3 min = quat_rotate_vec3(q, a.Min);
  //Vec3 max = quat_rotate_vec3(q, a.Max);

  //o[0] = min;
  o[0] = vec3_add(
            vec3_add(
              vec3_mul(x, a.Min.X), 
              vec3_mul(y, a.Min.Y)),
            vec3_mul(z, a.Min.Z));
  o[1] = vec3_add(o[0], vec3_mul(x, a.Max.X - a.Min.X));
  o[2] = vec3_add(o[0], vec3_mul(y, a.Max.Y - a.Min.Y));
  o[3] = vec3_add(o[0], vec3_mul(z, a.Max.Z - a.Min.Z));

  o[4] = vec3_add(
            vec3_add(
              vec3_mul(x, a.Max.X), 
              vec3_mul(y, a.Max.Y)),
            vec3_mul(z, a.Max.Z));
  o[5] = vec3_add(o[4], vec3_mul(x, a.Min.X - a.Max.X));
  o[6] = vec3_add(o[4], vec3_mul(y, a.Min.Y - a.Max.Y));
  o[7] = vec3_add(o[4], vec3_mul(z, a.Min.Z - a.Max.Z));
}
