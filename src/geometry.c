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
aabox_to_obox(AABox a, OBox o, Vec3 p, Quat q, Vec3 scale)
{
  Vec3 x = quat_rotate_vec3(q, vec3(1,0,0));
  Vec3 y = quat_rotate_vec3(q, vec3(0,1,0));
  Vec3 z = quat_rotate_vec3(q, vec3(0,0,1));

  a.Min = vec3_vec3_mul(a.Min, scale);
  a.Max = vec3_vec3_mul(a.Max, scale);

  o[0] = vec3_add(
            vec3_add(
              vec3_mul(x, a.Min.x), 
              vec3_mul(y, a.Min.y)),
            vec3_mul(z, a.Min.z));
  o[1] = vec3_add(o[0], vec3_mul(x, a.Max.x - a.Min.x));
  o[2] = vec3_add(o[0], vec3_mul(y, a.Max.y - a.Min.y));
  o[3] = vec3_add(o[0], vec3_mul(z, a.Max.z - a.Min.z));

  o[4] = vec3_add(
            vec3_add(
              vec3_mul(x, a.Max.x), 
              vec3_mul(y, a.Max.y)),
            vec3_mul(z, a.Max.z));
  o[5] = vec3_add(o[4], vec3_mul(x, a.Min.x - a.Max.x));
  o[6] = vec3_add(o[4], vec3_mul(y, a.Min.y - a.Max.y));
  o[7] = vec3_add(o[4], vec3_mul(z, a.Min.z - a.Max.z));

  int i = 0;
  for (i = 0; i<8; ++i) {
    o[i] = vec3_add(o[i], p);
  }
}

