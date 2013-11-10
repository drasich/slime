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

  a.min = vec3_vec3_mul(a.min, scale);
  a.max = vec3_vec3_mul(a.max, scale);

  o[0] = vec3_add(
            vec3_add(
              vec3_mul(x, a.min.x), 
              vec3_mul(y, a.min.y)),
            vec3_mul(z, a.min.z));
  o[1] = vec3_add(o[0], vec3_mul(x, a.max.x - a.min.x));
  o[2] = vec3_add(o[0], vec3_mul(y, a.max.y - a.min.y));
  o[3] = vec3_add(o[0], vec3_mul(z, a.max.z - a.min.z));

  o[4] = vec3_add(
            vec3_add(
              vec3_mul(x, a.max.x), 
              vec3_mul(y, a.max.y)),
            vec3_mul(z, a.max.z));
  o[5] = vec3_add(o[4], vec3_mul(x, a.min.x - a.max.x));
  o[6] = vec3_add(o[4], vec3_mul(y, a.min.y - a.max.y));
  o[7] = vec3_add(o[4], vec3_mul(z, a.min.z - a.max.z));

  int i = 0;
  for (i = 0; i<8; ++i) {
    o[i] = vec3_add(o[i], p);
  }
}

