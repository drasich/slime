#include "vec.h"
#include <math.h>

Vec3 
vec3_zero()
{
  Vec3 v = {
    .x = 0,
    .y = 0,
    .z = 0
  };

  return v;
}

Vec3
vec3(double x, double y, double z)
{
  Vec3 v = {
    .x = x,
    .y = y,
    .z = z
  };

  return v;
}

Vec4
vec4(double x, double y, double z, double w)
{
  Vec4 v = {
    .x = x,
    .y = y,
    .z = z,
    .w = w
  };

  return v;
}

double
vec3_length(Vec3 v)
{
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double
vec3_length2(Vec3 v)
{
  return v.x*v.x + v.y*v.y + v.z*v.z;
}

double
vec4_length(Vec4 v)
{
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

double
vec4_length2(Vec4 v)
{
  return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}

Vec3
vec3_add(Vec3 v1, Vec3 v2)
{
  Vec3 v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  v.z = v1.z + v2.z;
  return v;
}

Vec3
vec3_sub(Vec3 v1, Vec3 v2)
{
  Vec3 v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  v.z = v1.z - v2.z;
  return v;
}

Vec3
vec3_mul(Vec3 v1, double s)
{
  Vec3 v;
  v.x = v1.x*s;
  v.y = v1.y*s;
  v.z = v1.z*s;
  return v;
}

Vec3
vec3_cross(Vec3 v1, Vec3 v2)
{
  Vec3 v = {
    .x = v1.y * v2.z - v1.z*v2.y,
    .y = v1.z * v2.x - v1.x*v2.z,
    .z = v1.x * v2.y - v1.y*v2.x
  };

  return v;
}

double
vec3_dot(Vec3 v1, Vec3 v2)
{
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vec3
vec3_normalized(Vec3 v)
{
  Vec3 r = vec3_mul(v, 1.0f/vec3_length(v));
  return r;
}


double
vec4_dot(Vec4 v1, Vec4 v2)
{
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}


bool
vec3_equal(Vec3 v1, Vec3 v2)
{
  return v1.x == v2.x &&
   v1.y == v2.y &&
   v1.z == v2.z;
}

Vec3
vec3_vec3_mul(Vec3 v1, Vec3 v2)
{
  Vec3 r;
  r.x = v1.x * v2.x;
  r.y = v1.y * v2.y;
  r.z = v1.z * v2.z;
  return r;
}

bool
vec4_equal(Vec4 v1, Vec4 v2)
{
  return v1.x == v2.x &&
   v1.y == v2.y &&
   v1.z == v2.z &&
   v1.w == v1.w;
}

Vec3f
vec3d_to_vec3f(Vec3 v)
{
  Vec3f vf;
  vf.x = (float) v.x;
  vf.y = (float) v.y;
  vf.z = (float) v.z;
  return vf;

}
