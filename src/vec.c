#include "vec.h"
#include <math.h>

Vec3 
vec3_zero()
{
  Vec3 v = {
    .X = 0,
    .Y = 0,
    .Z = 0
  };

  return v;
}

Vec3
vec3(double x, double y, double z)
{
  Vec3 v = {
    .X = x,
    .Y = y,
    .Z = z
  };

  return v;
}

Vec4
vec4(double x, double y, double z, double w)
{
  Vec4 v = {
    .X = x,
    .Y = y,
    .Z = z,
    .W = w
  };

  return v;
}

double
vec3_length(Vec3 v)
{
  return sqrt(v.X*v.X + v.Y*v.Y + v.Z*v.Z);
}

double
vec3_length2(Vec3 v)
{
  return v.X*v.X + v.Y*v.Y + v.Z*v.Z;
}

double
vec4_length(Vec4 v)
{
  return sqrt(v.X*v.X + v.Y*v.Y + v.Z*v.Z + v.W*v.W);
}

double
vec4_length2(Vec4 v)
{
  return v.X*v.X + v.Y*v.Y + v.Z*v.Z + v.W*v.W;
}

Vec3
vec3_add(Vec3 v1, Vec3 v2)
{
  Vec3 v;
  v.X = v1.X + v2.X;
  v.Y = v1.Y + v2.Y;
  v.Z = v1.Z + v2.Z;
  return v;
}

Vec3
vec3_sub(Vec3 v1, Vec3 v2)
{
  Vec3 v;
  v.X = v1.X - v2.X;
  v.Y = v1.Y - v2.Y;
  v.Z = v1.Z - v2.Z;
  return v;
}

Vec3
vec3_mul(Vec3 v1, double s)
{
  Vec3 v;
  v.X = v1.X*s;
  v.Y = v1.Y*s;
  v.Z = v1.Z*s;
  return v;
}

Vec3
vec3_cross(Vec3 v1, Vec3 v2)
{
  Vec3 v = {
    .X = v1.Y * v2.Z - v1.Z*v2.Y,
    .Y = v1.Z * v2.X - v1.X*v2.Z,
    .Z = v1.X * v2.Y - v1.Y*v2.X
  };

  return v;
}

double
vec3_dot(Vec3 v1, Vec3 v2)
{
  return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
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
  return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z + v1.W*v2.W;
}


bool
vec3_equal(Vec3 v1, Vec3 v2)
{
  return v1.X == v2.X &&
   v1.Y == v2.Y &&
   v1.Z == v2.Z;
}

Vec3
vec3_vec3_mul(Vec3 v1, Vec3 v2)
{
  Vec3 r;
  r.X = v1.X * v2.X;
  r.Y = v1.Y * v2.Y;
  r.Z = v1.Z * v2.Z;
  return r;
}

bool
vec4_equal(Vec4 v1, Vec4 v2)
{
  return v1.X == v2.X &&
   v1.Y == v2.Y &&
   v1.Z == v2.Z &&
   v1.W == v1.W;
}

