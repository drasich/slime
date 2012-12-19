#include "vec.h"
#include <math.h>

double
vec3_length(Vec3 v)
{
  return sqrt(v.X*v.X + v.Y*v.Y + v.Z*v.Z);
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

IntersectionRayPlane
intersection_ray_plane(Ray ray, Plane plane)
{
  double dn = vec3_dot(ray.Direction, plane.Normal);
  IntersectionRayPlane out = { .hit = 0};

  if (dn != 0) {
    double d = -vec3_dot(plane.Normal, plane.Point);
    double p0n = vec3_dot(ray.Start, plane.Normal);
    double t = (d - p0n) / dn;
    out.hit = 1;
    out.position = vec3_add(ray.Start, vec3_mul(ray.Direction, t));
  }

  return out;
}
