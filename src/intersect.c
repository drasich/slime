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
    double d = vec3_dot(plane.Normal, plane.Point);
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
      return out;
    }
    xt /= ray.Direction.X;
    out.inside = false;
    xn = -1;
  } else if (ray.Start.X > box.Max.X) {
    xt = box.Max.X - ray.Start.X;
    if (xt < ray.Direction.X) {
      out.inside = false;
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
        out.inside = false; return out; }
      z = ray.Start.Z + ray.Direction.Z*t;
      if (z < box.Min.Z - FLT_EPSILON || z > box.Max.Z + FLT_EPSILON) {
        out.inside = false; return out; }

    out.normal.X = xn;
  case 1: //xz plane
    x = ray.Start.X + ray.Direction.X*t;
    if (x < box.Min.X - FLT_EPSILON || x > box.Max.X + FLT_EPSILON) { 
      out.inside = false; return out; }
    z = ray.Start.Z + ray.Direction.Z*t;
    if (z < box.Min.Z - FLT_EPSILON || z > box.Max.Z + FLT_EPSILON) {
      out.inside = false; return out; }

    out.normal.Y = yn;
  case 2:
    x = ray.Start.X + ray.Direction.X*t;
    if (x < box.Min.X - FLT_EPSILON || x > box.Max.X + FLT_EPSILON) {
      out.inside = false; return out; }
    y = ray.Start.Y + ray.Direction.Y*t;
    if (y < box.Min.Y - FLT_EPSILON || y > box.Max.Y + FLT_EPSILON) { 
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

  /*
  if (!(dot < 0.0)) {
    printf("normal : %f %f %f\n", n.X, n.Y, n.Z);
    printf("return dot < 0 : %f\n", dot);
    return out;
  }
  */

  double d = vec3_dot(n, t.v0);

  double tt = d - vec3_dot(n, r.Start);

  /*
  if (!(tt<= 0.0)) {
    printf("return tt <= 0\n");
    return out;
  }
  */

  //if (!(tt >= dot*min)) {
  if (!(tt/dot <= min)) { //TODO check this from the pdf
    //printf("return tt >= dot*min\n");
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
  if (!(temp != 0.0)) {
    //printf("return temp !=0 \n");
    return out;
  }

  temp = 1.0 / temp;

  double alpha = (a0 * b2 - b0 * a2) * temp;
  if (!(alpha >= 0.0)) {
    //printf("return alpha\n");
    return out;
  }

  double beta = (a1 * b0 - b1 * a0) * temp;
  if (!(beta >= 0.0)) { 
    //printf("return beta\n");
    return out;
  }

  float gamma = 1.0 - alpha - beta;
  if (!(gamma >= 0.0)) {
    //printf("return gamma\n");
    return out;
  }

  out.hit = true;
  out.position = p;

  return out;

}

IntersectionRay
intersection_ray_object(Ray ray, Object* o)
{
  IntersectionRay out = { .hit = false};

  Mesh* m = o->mesh;
  if (m == NULL) return out;

  Repere r = {o->Position, o->Orientation};
  Ray newray;
  newray.Start = world_to_local(r, ray.Start);
  newray.Direction = world_to_local(r, vec3_add(ray.Direction, ray.Start));
  newray.Direction = vec3_sub(newray.Direction, newray.Start);

  int i;
  for (i = 0; i < m->indices_len; i+=3) {
    int id = m->indices[i];
    Vec3 v0 = { 
      m->vertices[id*3],
      m->vertices[id*3 + 1],
      m->vertices[id*3 + 2]
    };
    id = m->indices[i+1];
    Vec3 v1 = { 
      m->vertices[id*3],
      m->vertices[id*3 + 1],
      m->vertices[id*3 + 2]
    };
    id = m->indices[i+2];
    Vec3 v2 = { 
      m->vertices[id*3],
      m->vertices[id*3 + 1],
      m->vertices[id*3 + 2]
    };
    Triangle tri = { v0, v1, v2};
    out = intersection_ray_triangle(newray,tri,1);
    if (out.hit) {
      //transform back
      out.position = local_to_world(r, out.position);
      out.normal = quat_rotate_vec3(o->Orientation, out.normal);
      return out;
    }
  }

  return out;
}

bool
frustum_is_in(Frustum* f, Vec3 p)
{
  Vec3 v = vec3_sub(p, f->start);
  Vec3 dirn = vec3_normalized(f->direction);
  float pcz = vec3_dot(v, dirn);
  if (pcz > f->far || pcz < f->near) {
    return false;
  }

  Vec3 right = vec3_cross(f->direction, f->up);

  float pcx = vec3_dot(v, right);
  float pcy = vec3_dot(v, f->up);
  
  float h = pcz * 2.0f * tan(f->fovy/2.0f);
  if ( -h/2.0f > pcy || pcy > h/2.0f)
  return false;


  //float w = pcz * 2.0f * tan(f->fovx/2.0f);
  //float w = h * f->fovx/f->fovy;
  float w = h * f->aspect;
  //printf("float w, pcz :  : %f, %f \n", w, pcx);
  if ( -w/2.0f > pcx || pcx > w/2.0f)
  return false;

  return true;
}

bool frustum_is_in_rect(
      Frustum* f,
      Vec3 p,
      float left,
      float top,
      float width,
      float height)
{

  return true;
}

bool
plane_is_in(Plane p, Vec3 v)
{
  /*
  printf("testing plane \n");
  printf(" point : %f, %f, %f\n", p.Point.X, p.Point.Y, p.Point.Z);
  printf(" normal : %f, %f, %f\n", p.Normal.X, p.Normal.Y, p.Normal.Z);
  printf(" with v : %f, %f, %f\n", v.X, v.Y, v.Z);
  */
  Vec3 pos = vec3_sub(v, p.Point);
  //printf(" pos is : %f, %f, %f\n", pos.X, pos.Y, pos.Z);
  float dot = vec3_dot(pos, p.Normal);
  //printf("dot is : %f \n", dot);
  return dot >= 0;
}

bool planes_is_in(Plane* p, int nb_planes, Vec3 v)
{
  int i = 0;
  for (i = 0; i < nb_planes; ++i) {
    printf("PLANE %d\n ", i);
    if (!plane_is_in(p[i], v)) return false;
  }

  return true;
}


bool
frustum_is_box_in_false_positives(Frustum* f, OBox b)
{
  //TODO first test if one point of the box is inside of frustum with the radar test
  //if yes then return true
  int i;
  for (i = 0; i < 8; ++i){
    if (frustum_is_in(f, b[i])) return true;
  }

  //we didn't return so do planes is in box (function below)

  return false;
}

bool
planes_is_box_in_allow_false_positives(Plane* p, int nb_planes, OBox b)
{
	int i, out, in;

	// for each plane do ...
	for(i=0; i < nb_planes; ++i) {

		out=0;
    in=0;
		// for each corner of the box do ...
		// get out of the cycle as soon as a box as corners
		// both inside and out of the frustum
    int k;
		for (k = 0; k < 8 && (in==0 || out==0); k++) {

			// is the corner on the good or bad side of the plane
      if (plane_is_in(p[i], b[k]))
        ++in;
      else
        ++out;
		}
		//if all corners are out
		if (!in)
			return false;
	}
	return true;
}


bool
planes_is_in_object(Plane* p, int nb_planes, Object* o)
{
  Mesh* m = o->mesh;
  if (!m) return false;

  Repere r = {o->Position, o->Orientation};
  Quat iq = quat_conj(r.rotation);

  int i;
  for (i = 0; i< nb_planes; i++) {
    Vec3 point = p[i].Point;
    p[i].Point =  world_to_local(r, point);
    //p[i].Normal = quat_rotate_vec3(iq,p[i].Normal);
    p[i].Normal = world_to_local(r, vec3_add(p[i].Normal, point));
    p[i].Normal = vec3_sub(p[i].Normal, p[i].Point);

    //newray.Direction = world_to_local(r, vec3_add(ray.Direction, ray.Start));
    //newray.Direction = vec3_sub(newray.Direction, newray.Start);
  }

  for (i = 0; i < m->indices_len; i+=3) {
    int id = m->indices[i];
    Vec3 v0 = { 
      m->vertices[id*3],
      m->vertices[id*3 + 1],
      m->vertices[id*3 + 2]
    };
    id = m->indices[i+1];
    Vec3 v1 = { 
      m->vertices[id*3],
      m->vertices[id*3 + 1],
      m->vertices[id*3 + 2]
    };
    id = m->indices[i+2];
    Vec3 v2 = { 
      m->vertices[id*3],
      m->vertices[id*3 + 1],
      m->vertices[id*3 + 2]
    };
    Triangle tri = { v0, v1, v2};
    if (planes_is_in_triangle(p, nb_planes, tri)) return true;
  }

  return false;
}


static IntersectionPlaneTriangle
_intersection_plane_triangle(Plane p, Triangle t)
{
  IntersectionPlaneTriangle ipt = { false, { vec3_zero(), vec3_zero()} };

  bool b0 = plane_is_in(p, t.v0);
  bool b1 = plane_is_in(p, t.v1);
  bool b2 = plane_is_in(p, t.v2);

  Ray r0, r1;

  if (b0 != b1) {
    ipt.intersect = true;
    if (b0 != b2) {
      //bo is alone
      r0.Start = t.v0;
      r0.Direction = vec3_sub(t.v1, t.v0);
      r1.Start = t.v0;
      r1.Direction = vec3_sub(t.v2, t.v0);
    }
    else {
      //b1 is alone
      r0.Start = t.v1;
      r0.Direction = vec3_sub(t.v0, t.v1);
      r1.Start = t.v1;
      r1.Direction = vec3_sub(t.v2, t.v1);
    }
  }
  else if ( b0 != b2){
    ipt.intersect = true;
    //b2 is alone
    r0.Start = t.v2;
    r0.Direction = vec3_sub(t.v0, t.v2);
    r1.Start = t.v2;
    r1.Direction = vec3_sub(t.v1, t.v2);
  }

  if (ipt.intersect) {
    IntersectionRay ir0 = intersection_ray_plane(r0, p);
    IntersectionRay ir1 = intersection_ray_plane(r1, p);
    ipt.segment.p0 = ir0.position;
    ipt.segment.p1 = ir1.position;
  }

  return ipt;
}

bool
planes_is_in_triangle(Plane* p, int nb_planes, Triangle t)
{
  int i;
  bool point_is_in = true;
  for (i = 0; i< nb_planes; i++) {
    if (!plane_is_in(p[i], t.v0)) {
      point_is_in = false;
      break;
    }
  }

  if (point_is_in) return true;

  point_is_in = true;
  for (i = 0; i< nb_planes; i++) {
    if (!plane_is_in(p[i], t.v1)) {
      point_is_in = false;
      break;
    }
  }

  if (point_is_in) return true;

  point_is_in = true;
  for (i = 0; i< nb_planes; i++) {
    if (!plane_is_in(p[i], t.v2)) {
      point_is_in = false;
      break;
    }
  }

  if (point_is_in) return true;

  //TODO stuff here when points are not inside the plan
  // like axis separating test

  for (i = 0; i< nb_planes; i++) {
    IntersectionPlaneTriangle ipt = _intersection_plane_triangle(p[i], t);
    if (ipt.intersect) {
      //
      //there is at least one point in the plane
      //we need to test if the intersection of the
      //plane and the triangle is in the frustum

      //check if segment intersect the plane or frustum
      //il ne suffit pas de juste tester les points
      
    }
  }

  return false;
}

