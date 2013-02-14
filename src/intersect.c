#include "intersect.h"
#include <stdio.h>
#include <math.h>

IntersectionRay
intersection_ray_plane(Ray ray, Plane plane)
{
  double dn = vec3_dot(ray.Direction, plane.Normal);
  IntersectionRay out = { .hit = 0};

  if (dn != 0) {
    double d = -vec3_dot(plane.Normal, plane.Point);
    double p0n = vec3_dot(ray.Start, plane.Normal);
    double t = (d - p0n) / dn;
    out.hit = 1;
    out.position = vec3_add(ray.Start, vec3_mul(ray.Direction, t));
  }

  return out;
}

IntersectionRay
intersection_ray_sphere(Ray ray, Sphere sphere)
{
  IntersectionRay out = { .hit = 0};

  Vec3 e = vec3_sub(sphere.center, ray.Start);
  float a = vec3_dot(e,ray.Direction);
  float radius = sphere.radius;
  float f2 = radius*radius - vec3_length2(e) + a*a;
  if ( f2 >= 0) {
    float t = a - sqrt(f2);
    if (t > 0) {
      out.hit = 1;
      out.position = vec3_add(ray.Start, vec3_mul(ray.Direction, t));
    }
    //else {
      //printf("negative\n");
    //}
  } //else printf("no collision with\n");

  return out;
}
