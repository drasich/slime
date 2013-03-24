#include "intersect.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <float.h>

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

  if (out.inside) {
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

  //printf("position %f, %f, %f \n", ir.position.X, ir.position.Y, ir.position.Z);

  double x, y, z;

  switch (which) {
    case 0: // yz plane
      y = ray.Start.Y + ray.Direction.Y*t;
      if (y < box.Min.Y - FLT_EPSILON || y > box.Max.Y + FLT_EPSILON) { 
        printf("return case 0_1\n");
        out.inside = false; return out; }
      z = ray.Start.Z + ray.Direction.Z*t;
      if (z < box.Min.Z - FLT_EPSILON || z > box.Max.Z + FLT_EPSILON) {
        printf("return case 0_2\n");
        out.inside = false; return out; }

    out.normal.X = xn;
  case 1: //xz plane
    x = ray.Start.X + ray.Direction.X*t;
    if (x < box.Min.X - FLT_EPSILON || x > box.Max.X + FLT_EPSILON) { 
        printf("return case 1_1\n");
      out.inside = false; return out; }
    z = ray.Start.Z + ray.Direction.Z*t;
    if (z < box.Min.Z - FLT_EPSILON || z > box.Max.Z + FLT_EPSILON) {
        printf("return case 1_2\n");
      out.inside = false; return out; }

    out.normal.Y = yn;
  case 2:
    x = ray.Start.X + ray.Direction.X*t;
    if (x < box.Min.X - FLT_EPSILON || x > box.Max.X + FLT_EPSILON) {
      printf("return case 2_1\n");
      out.inside = false; return out; }
    y = ray.Start.Y + ray.Direction.Y*t;
    if (y < box.Min.Y - FLT_EPSILON || y > box.Max.Y + FLT_EPSILON) { 
      printf("return case 2_2 : %4.16f %4.16f %4.16f %4.16f \n", y, box.Min.Y, box.Max.Y);
      out.inside = false; return out; }

    out.normal.Y = zn;
  }

  out.position = vec3_add(ray.Start, vec3_mul(ray.Direction,t));
  out.hit = true;
 
  return out;
}

IntersectionRay
intersection_ray_box(Ray ray, AABox box, Vec3 position, Quat rotation)
{
  Repere r = {position, rotation};
  //transform the ray in box/object coord
  Ray newray;
  newray.Start = world_to_local(r, ray.Start);
  newray.Direction = world_to_local(r, vec3_add(ray.Direction, ray.Start));
  newray.Direction = vec3_sub(newray.Direction, newray.Start);

  IntersectionRay ir = intersection_ray_aabox(newray, box);

  //transform back
  ir.position = local_to_world(r, ir.position);
  ir.normal = quat_rotate_vec3(rotation, ir.normal);

  return ir;
}

Vec3
world_to_local(Repere r, Vec3 v)
{
  Vec3 out = vec3_sub(v, r.origin);
  Quat iq = quat_conj(r.rotation);
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


IntersectionRay
intersection_ray_triangle(Ray r, Triangle t, double min)
{
  IntersectionRay out = { .hit = false};
  const double NO_INT = DBL_MAX;

  Vec3 e1 = vec3_sub(t.v1, t.v0);
  Vec3 e2 = vec3_sub(t.v2, t.v1);

  Vec3 n = vec3_cross(e1,e2);

  double dot = vec3_dot(n, r.Direction);

  if (!(dot < 0.0)) {
    return out;
  }

  double d = vec3_dot(n, t.v0);

  double tt = d - vec3_dot(n, r.Start);

  if (!(tt<= 0.0)) return out;

  if (!(tt >= dot*min)) {
    return out;
  }

  tt /= dot;
  //assert(tt >= 0.0);
  //assert(tt <= min);

  Vec3 p = vec3_add(r.Start, vec3_mul(r.Direction, tt));

  double a0, a1, a2;
  double b0, b1, b2;
  if ( fabs(n.X) > fabs(n.Y)) {
    if (fabs(n.X) > fabs(n.Z)) {
      a0 = p.Y - t.v0.Y;
      a1 = t.v1.Y - t.v0.Y;
      a2 = t.v2.Y - t.v0.Y;

      b0 = p.Z - t.v0.Z;
      b1 = t.v1.Z - t.v0.Z;
      b2 = t.v2.Z - t.v0.Z;
    } else {
      a0 = p.X - t.v0.X;
      a1 = t.v1.X - t.v0.X;
      a2 = t.v2.X - t.v0.X;

      b0 = p.Y - t.v0.Y;
      b1 = t.v1.Y - t.v0.Y;
      b2 = t.v2.Y - t.v0.Y;
    }
  } else {
    if (fabs(n.Y) > fabs(n.Z)) {
      a0 = p.X - t.v0.X;
      a1 = t.v1.X - t.v0.X;
      a2 = t.v2.X - t.v0.X;

      b0 = p.Z - t.v0.Z;
      b1 = t.v1.Z - t.v0.Z;
      b2 = t.v2.Z - t.v0.Z;
    } else {
      a0 = p.X - t.v0.X;
      a1 = t.v1.X - t.v0.X;
      a2 = t.v2.X - t.v0.X;

      b0 = p.Y - t.v0.Y;
      b1 = t.v1.Y - t.v0.Y;
      b2 = t.v2.Y - t.v0.Y;
    }
  }

  double temp = a1* b2 - b1*a2;
  if (!(temp != 0.0)) return out;

  temp = 1.0 / temp;

  double alpha = (a0 * b2 - b0 * a2) * temp;
  if (!(alpha >= 0.0)) return out;

  double beta = (a1 * b0 - b1 * a0) * temp;
  if (!(beta >= 0.0)) return out;

  float gamma = 1.0 - alpha - beta;
  if (!(gamma >= 0.0)) {
    return out;
  }

  out.hit = true;
  out.position = p;

  return out;

}

IntersectionRay
intersection_ray_mesh(Ray ray, Mesh* m)
{
  int i;
  for (i = 0; i < m->indices_len / 3; ++i) {
    Vec3 v0 = { 
      m->vertices[i*9],
      m->vertices[i*9 + 1],
      m->vertices[i*9 + 2]
    };
    Vec3 v1 = { 
      m->vertices[i*9 + 3],
      m->vertices[i*9 + 4],
      m->vertices[i*9 + 5]
    };
    Vec3 v2 = { 
      m->vertices[i*9 + 6],
      m->vertices[i*9 + 7],
      m->vertices[i*9 + 8]
    };
    Triangle tri = { v0, v1, v2};

    IntersectionRay ir = intersection_ray_triangle(ray,tri,1);
    if (ir.hit) return ir;
  }

  IntersectionRay out = { .hit = false};
  return out;

}
