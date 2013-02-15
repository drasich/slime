#include "intersect.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>

IntersectionRay
intersection_ray_plane(Ray ray, Plane plane)
{
  double dn = vec3_dot(ray.Direction, plane.Normal);
  IntersectionRay out = { .hit = false};

  if (dn != 0) {
    double d = -vec3_dot(plane.Normal, plane.Point);
    double p0n = vec3_dot(ray.Start, plane.Normal);
    double t = (d - p0n) / dn;
    out.hit = true;
    out.position = vec3_add(ray.Start, vec3_mul(ray.Direction, t));
  }

  return out;
}

IntersectionRay
intersection_ray_sphere(Ray ray, Sphere sphere)
{
  IntersectionRay out = { .hit = false};

  Vec3 e = vec3_sub(sphere.center, ray.Start);
  float a = vec3_dot(e,ray.Direction);
  float radius = sphere.radius;
  float f2 = radius*radius - vec3_length2(e) + a*a;
  if ( f2 >= 0) {
    float t = a - sqrt(f2);
    if (t > 0) {
      out.hit = true;
      out.position = vec3_add(ray.Start, vec3_mul(ray.Direction, t));
    }
    //else {
      //printf("negative\n");
    //}
  } //else printf("no collision with\n");

  return out;
}

IntersectionRay
intersection_ray_aabox(Ray ray, AABox box)
{
  IntersectionRay out = { .hit = false, .inside = true};
  printf("pass 00 \n");

  double xt, xn;

  if (ray.Start.X < box.Min.X) {
    xt = box.Min.X - ray.Start.X;
    if (xt > ray.Direction.X) {
      out.inside = false;
      printf("return 00 \n");
      return out;
    }
    xt /= ray.Direction.X;
    out.inside = false;
    xn = -1;
  } else if (ray.Start.X > box.Max.X) {
    xt = box.Max.X - ray.Start.X;
    if (xt < ray.Direction.X) {
      out.inside = false;
      printf("return 01 \n");
      return out;
    }
    xt /= ray.Direction.X;
    out.inside = false;
    xn = 1;
  } else {
    xt = -1;
  }

  printf("pass 10 \n");

  double yt, yn;
  if (ray.Start.Y < box.Min.Y) {
    yt = box.Min.Y - ray.Start.Y;
    if (yt > ray.Direction.Y) {
      out.inside = false;
      return out;
    }
    yt /= ray.Direction.Y;
    out.inside = false;
    yn = -1;
  } else if (ray.Start.Y > box.Max.Y) {
    yt = box.Max.Y - ray.Start.Y;
    if (yt < ray.Direction.Y) {
      out.inside = false;
      return out;
    }
    yt /= ray.Direction.Y;
    out.inside = false;
    yn = 1;
  } else {
    yt = -1;
  }

  printf("pass 20 \n");

  double zt, zn;
  if (ray.Start.Z < box.Min.Z) {
    zt = box.Min.Z - ray.Start.Z;
    if (zt > ray.Direction.Z) {
      out.inside = false;
      return out;
    }
    zt /= ray.Direction.Z;
    out.inside = false;
    zn = -1;
  } else if (ray.Start.Z > box.Max.Z) {
    zt = box.Max.Z - ray.Start.Z;
    if (zt < ray.Direction.Z) {
      out.inside = false;
      return out;
    }
    zt /= ray.Direction.Z;
    out.inside = false;
    zn = 1;
  } else {
    zt = -1;
  }

  printf("pass 30 \n");

  if (out.inside) {
    printf("we are inside!!!! \n");
    out.hit = true;
    return out;
  }

  uint16_t which = 0;
  double t = xt;
  if (yt > t) {
    which = 1;
    t = yt;
  }
  if (zt > t) {
    which = 2;
    t = zt;
  }

  printf("did you arrive here\n");
  //printf("posistion %f, %f, %f \n", ir.position.X, ir.position.Y, ir.position.Z);

  double x, y, z;

  switch (which) {
    case 0: // yz plane
      y = ray.Start.Y + ray.Direction.Y*t;
      if (y < box.Min.Y || y > box.Max.Y) { out.inside = false; return out; }
      z = ray.Start.Z + ray.Direction.Z*t;
      if (z < box.Min.Z || z > box.Max.Z) { out.inside = false; return out; }

    out.normal.X = xn;
  case 1: //xz plane
    x = ray.Start.X + ray.Direction.X*t;
    if (x < box.Min.X || x > box.Max.X) { out.inside = false; return out; }
    z = ray.Start.Z + ray.Direction.Z*t;
    if (z < box.Min.Z || z > box.Max.Z) { out.inside = false; return out; }

    out.normal.Y = yn;
  case 2:
    x = ray.Start.X + ray.Direction.X*t;
    if (x < box.Min.X || x > box.Max.X) { out.inside = false; return out; }
    y = ray.Start.Y + ray.Direction.Y*t;
    if (y < box.Min.Y || y > box.Max.Y) { out.inside = false; return out; }

    out.normal.Y = zn;
  }

  out.position = vec3_add(ray.Start, vec3_mul(ray.Direction,t));
  out.hit = true;
 
  return out;
}

IntersectionRay
intersection_ray_box(Ray ray, AABox box, Vec3 position, Quat rotation)
{
  /*
  Repere r = { vec3(1,1,0), quat_angle_axis(3.141519/2, vec3(0,0,1))};
  Vec3 a = vec3(2,2,0);
  a = world_to_local(r, a);
  printf("local to world : %f %f %f\n", a.X, a.Y, a.Z);
  */

  Repere r = {position, rotation};
  //transform the ray in box/object coord
  Ray newray;
  newray.Start = world_to_local(r, ray.Start);
  newray.Direction = world_to_local(r, vec3_add(ray.Direction, ray.Start));
  newray.Direction = vec3_sub(newray.Direction, newray.Start);
          

  IntersectionRay ir = intersection_ray_aabox(newray, box);

  //transform back
  //Quat iq = quat_conj(rotation);
  //Quat iq = quat_inverse(rotation);
  //ir.position = quat_rotate_vec3(rotation, ir.position);// o.Orientation.RotateVec3(position)
  //ir.position = quat_rotate_vec3(iq, ir.position);// o.Orientation.RotateVec3(position)
  //ir.position = vec3_add(ir.position, position);

  //ir.normal = quat_rotate_vec3(rotation, ir.normal); //o.Orientation.RotateVec3(normal)

  return ir;
}

Vec3
world_to_local(Repere r, Vec3 v)
{
  Vec3 out = vec3_sub(v, r.origin);
  Quat iq = quat_inverse(r.rotation);
  out = quat_rotate_vec3(iq, out);
  return out;
}

Vec3
local_to_world(Repere r, Vec3 v)
{
  Vec3 out = quat_rotate_vec3(r.rotation, v);
  out = vec3_add(out, r.origin);
  return out;
}

