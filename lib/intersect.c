#include "intersect.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <float.h>
#include "component/meshcomponent.h"

IntersectionRay
intersection_ray_plane(Ray ray, Plane plane)
{
  double dn = vec3_dot(ray.direction, plane.normal);
  IntersectionRay out = { .hit = false};

  if (dn != 0) {
    double d = vec3_dot(plane.normal, plane.point);
    double p0n = vec3_dot(ray.start, plane.normal);
    double t = (d - p0n) / dn;
    out.hit = true;
    out.position = vec3_add(ray.start, vec3_mul(ray.direction, t));
  }

  return out;
}

IntersectionRay
intersection_ray_sphere(Ray ray, Sphere sphere)
{
  IntersectionRay out = { .hit = false};

  Vec3 e = vec3_sub(sphere.center, ray.start);
  float a = vec3_dot(e,ray.direction);
  float radius = sphere.radius;
  float f2 = radius*radius - vec3_length2(e) + a*a;
  if ( f2 >= 0) {
    float t = a - sqrt(f2);
    if (t > 0) {
      out.hit = true;
      out.position = vec3_add(ray.start, vec3_mul(ray.direction, t));
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

  if (ray.start.x < box.min.x) {
    xt = box.min.x - ray.start.x;
    if (xt > ray.direction.x) {
      out.inside = false;
      return out;
    }
    xt /= ray.direction.x;
    out.inside = false;
    xn = -1;
  } else if (ray.start.x > box.max.x) {
    xt = box.max.x - ray.start.x;
    if (xt < ray.direction.x) {
      out.inside = false;
      return out;
    }
    xt /= ray.direction.x;
    out.inside = false;
    xn = 1;
  } else {
    xt = -1;
  }

  double yt, yn;
  if (ray.start.y < box.min.y) {
    yt = box.min.y - ray.start.y;
    if (yt > ray.direction.y) {
      out.inside = false;
      return out;
    }
    yt /= ray.direction.y;
    out.inside = false;
    yn = -1;
  } else if (ray.start.y > box.max.y) {
    yt = box.max.y - ray.start.y;
    if (yt < ray.direction.y) {
      out.inside = false;
      return out;
    }
    yt /= ray.direction.y;
    out.inside = false;
    yn = 1;
  } else {
    yt = -1;
  }

  double zt, zn;
  if (ray.start.z < box.min.z) {
    zt = box.min.z - ray.start.z;
    if (zt > ray.direction.z) {
      out.inside = false;
      return out;
    }
    zt /= ray.direction.z;
    out.inside = false;
    zn = -1;
  } else if (ray.start.z > box.max.z) {
    zt = box.max.z - ray.start.z;
    if (zt < ray.direction.z) {
      out.inside = false;
      return out;
    }
    zt /= ray.direction.z;
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

  //printf("position %f, %f, %f \n", ir.position.x, ir.position.y, ir.position.z);

  double x, y, z;

  switch (which) {
    case 0: // yz plane
      y = ray.start.y + ray.direction.y*t;
      if (y < box.min.y - FLT_EPSILON || y > box.max.y + FLT_EPSILON) { 
        out.inside = false; return out; }
      z = ray.start.z + ray.direction.z*t;
      if (z < box.min.z - FLT_EPSILON || z > box.max.z + FLT_EPSILON) {
        out.inside = false; return out; }

    out.normal.x = xn;
  case 1: //xz plane
    x = ray.start.x + ray.direction.x*t;
    if (x < box.min.x - FLT_EPSILON || x > box.max.x + FLT_EPSILON) { 
      out.inside = false; return out; }
    z = ray.start.z + ray.direction.z*t;
    if (z < box.min.z - FLT_EPSILON || z > box.max.z + FLT_EPSILON) {
      out.inside = false; return out; }

    out.normal.y = yn;
  case 2:
    x = ray.start.x + ray.direction.x*t;
    if (x < box.min.x - FLT_EPSILON || x > box.max.x + FLT_EPSILON) {
      out.inside = false; return out; }
    y = ray.start.y + ray.direction.y*t;
    if (y < box.min.y - FLT_EPSILON || y > box.max.y + FLT_EPSILON) { 
      out.inside = false; return out; }

    out.normal.y = zn;
  }

  out.position = vec3_add(ray.start, vec3_mul(ray.direction,t));
  out.hit = true;
 
  return out;
}

IntersectionRay
intersection_ray_box(Ray ray, AABox box, Vec3 position, Quat rotation, Vec3 scale)
{
  Repere r = {position, rotation};
  //transform the ray in box/object coord
  Ray newray;
  newray.start = world_to_local(r, ray.start);
  newray.direction = world_to_local(r, vec3_add(ray.direction, ray.start));
  newray.direction = vec3_sub(newray.direction, newray.start);

  box.min = vec3_vec3_mul(box.min, scale);
  box.max = vec3_vec3_mul(box.max, scale);

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

  double dot = vec3_dot(n, r.direction);

  /*
  if (!(dot < 0.0)) {
    printf("normal : %f %f %f\n", n.x, n.y, n.z);
    printf("return dot < 0 : %f\n", dot);
    return out;
  }
  */

  double d = vec3_dot(n, t.v0);

  double tt = d - vec3_dot(n, r.start);

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

  Vec3 p = vec3_add(r.start, vec3_mul(r.direction, tt));

  double a0, a1, a2;
  double b0, b1, b2;
  if ( fabs(n.x) > fabs(n.y)) {
    if (fabs(n.x) > fabs(n.z)) {
      a0 = p.y - t.v0.y;
      a1 = t.v1.y - t.v0.y;
      a2 = t.v2.y - t.v0.y;

      b0 = p.z - t.v0.z;
      b1 = t.v1.z - t.v0.z;
      b2 = t.v2.z - t.v0.z;
    } else {
      a0 = p.x - t.v0.x;
      a1 = t.v1.x - t.v0.x;
      a2 = t.v2.x - t.v0.x;

      b0 = p.y - t.v0.y;
      b1 = t.v1.y - t.v0.y;
      b2 = t.v2.y - t.v0.y;
    }
  } else {
    if (fabs(n.y) > fabs(n.z)) {
      a0 = p.x - t.v0.x;
      a1 = t.v1.x - t.v0.x;
      a2 = t.v2.x - t.v0.x;

      b0 = p.z - t.v0.z;
      b1 = t.v1.z - t.v0.z;
      b2 = t.v2.z - t.v0.z;
    } else {
      a0 = p.x - t.v0.x;
      a1 = t.v1.x - t.v0.x;
      a2 = t.v2.x - t.v0.x;

      b0 = p.y - t.v0.y;
      b1 = t.v1.y - t.v0.y;
      b2 = t.v2.y - t.v0.y;
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
intersection_ray_mesh(Ray ray, Mesh* m, Vec3 position, Quat rotation, Vec3 scale)
{
  IntersectionRay out = { .hit = false};

  Repere r = {position, rotation};
  Ray newray;
  newray.start = world_to_local(r, ray.start);
  newray.direction = world_to_local(r, vec3_add(ray.direction, ray.start));
  newray.direction = vec3_sub(newray.direction, newray.start);

  int i;
  for (i = 0; i < m->indices_len; i+=3) {
    int id = m->indices[i];
    Vec3 v0 = mesh_vertex_get(m, id);
    id = m->indices[i+1];
    Vec3 v1 = mesh_vertex_get(m, id);
    id = m->indices[i+2];
    Vec3 v2 = mesh_vertex_get(m, id);

    v0 = vec3_vec3_mul(v0, scale);
    v1 = vec3_vec3_mul(v1, scale);
    v2 = vec3_vec3_mul(v2, scale);

    Triangle tri = {v0, v1, v2};
    out = intersection_ray_triangle(newray,tri,1);
    if (out.hit) {
      //transform back
      out.position = local_to_world(r, out.position);
      out.normal = quat_rotate_vec3(rotation, out.normal);
      return out;
    }
  }

  return out;

}

IntersectionRay
intersection_ray_object(Ray ray, Object* o)
{
  IntersectionRay out = { .hit = false};

  MeshComponent* mc = object_component_get(o,"mesh");
  if (!mc) {
    return out;
  }

  Mesh* m = mesh_component_mesh_get(mc);
  if (!m) {
    printf("no mesh %s \n", o->name);
    return out;
  }

  Vec3 wp = object_world_position_get(o);
  Quat wq = object_world_orientation_get(o);
  Vec3 ws = object_world_scale_get(o);

  //IntersectionRay ir_box = intersection_ray_box(ray, m->box, o->position, o->orientation, o->scale);
  IntersectionRay ir_box = intersection_ray_box(ray, m->box, wp, wq, ws);
  if (!ir_box.hit) {
    return out;
  }

  //return intersection_ray_mesh(ray, m, o->position, o->orientation, o->scale);
  return intersection_ray_mesh(ray, m, wp, wq, ws);

  //TODO perf: we compute the repere 2 times in box and then mesh.
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
  printf(" point : %f, %f, %f\n", p.point.x, p.point.y, p.point.z);
  printf(" normal : %f, %f, %f\n", p.normal.x, p.normal.y, p.normal.z);
  printf(" with v : %f, %f, %f\n", v.x, v.y, v.z);
  */
  Vec3 pos = vec3_sub(v, p.point);
  //printf(" pos is : %f, %f, %f\n", pos.x, pos.y, pos.z);
  float dot = vec3_dot(pos, p.normal);
  //printf("dot is : %f \n", dot);
  return dot >= 0;
}

bool planes_is_in(const Plane* p, int nb_planes, Vec3 v)
{
  int i = 0;
  for (i = 0; i < nb_planes; ++i) {
    //printf("PLANE %d\n ", i);
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
planes_is_box_in_allow_false_positives(const Plane* p, int nb_planes, OBox b)
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
planes_is_in_object(const Plane* planes, int nb_planes, const Object* o)
{
  MeshComponent* mc = object_component_get(o, "mesh");
  if (!mc)
  return planes_is_in(planes, 6, o->position);

  Mesh* m = mesh_component_mesh_get(mc);
  if (!m)
  return planes_is_in(planes, 6, o->position);

  //first test the box and then test the object/mesh
  OBox b;
  aabox_to_obox(m->box, b, o->position, o->orientation, o->scale);
  if (!planes_is_box_in_allow_false_positives(planes, 6, b)) return false;

  Plane p[nb_planes];
  memcpy (p, planes, nb_planes*sizeof *p);

  Repere r = {o->position, o->orientation};
  Quat iq = quat_conj(r.rotation);

  int i;
  for (i = 0; i< nb_planes; i++) {
    Vec3 point = p[i].point;
    p[i].point =  world_to_local(r, point);
    //p[i].normal = quat_rotate_vec3(iq,p[i].normal);
    p[i].normal = world_to_local(r, vec3_add(p[i].normal, point));
    p[i].normal = vec3_sub(p[i].normal, p[i].point);

    //newray.direction = world_to_local(r, vec3_add(ray.direction, ray.start));
    //newray.direction = vec3_sub(newray.direction, newray.start);
  }

  for (i = 0; i < m->indices_len; i+=3) {
    int id = m->indices[i];
    Vec3 v0 = mesh_vertex_get(m, id);
    id = m->indices[i+1];
    Vec3 v1 = mesh_vertex_get(m, id);
    id = m->indices[i+2];
    Vec3 v2 = mesh_vertex_get(m, id);

    v0 = vec3_vec3_mul(v0, o->scale);
    v1 = vec3_vec3_mul(v1, o->scale);
    v2 = vec3_vec3_mul(v2, o->scale);

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
      r0.start = t.v0;
      r0.direction = vec3_sub(t.v1, t.v0);
      r1.start = t.v0;
      r1.direction = vec3_sub(t.v2, t.v0);
    }
    else {
      //b1 is alone
      r0.start = t.v1;
      r0.direction = vec3_sub(t.v0, t.v1);
      r1.start = t.v1;
      r1.direction = vec3_sub(t.v2, t.v1);
    }
  }
  else if ( b0 != b2){
    ipt.intersect = true;
    //b2 is alone
    r0.start = t.v2;
    r0.direction = vec3_sub(t.v0, t.v2);
    r1.start = t.v2;
    r1.direction = vec3_sub(t.v1, t.v2);
  }

  if (ipt.intersect) {
    IntersectionRay ir0 = intersection_ray_plane(r0, p);
    IntersectionRay ir1 = intersection_ray_plane(r1, p);
    ipt.segment.p0 = ir0.position;
    ipt.segment.p1 = ir1.position;
  }

  return ipt;
}

static bool
_check_inter(Plane* p, int nb_planes, int notthisplane, int notthisplaneeither, Segment s)
{
  int i;
  for (i = 0; i< nb_planes; i++) {
    if (i == notthisplane || i == notthisplaneeither)
    continue;

    double planedot = vec3_dot(p[i].normal, p[i].point);
    double s0dot = vec3_dot(p[i].normal, s.p0);
    double s1dot = vec3_dot(p[i].normal, s.p1);

    if (s0dot >= planedot || s1dot >= planedot)
      continue;
    else return false;
  }
  //we tested all the planes and can return true
  return true;
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
    //test everything but the current plane
    if (ipt.intersect) {
      
      int otherplane = i % 2 == 0? i+1: i-1;
      //near, far, up, down, right, left

      if (_check_inter(p, nb_planes, i, otherplane, ipt.segment)) return true;
    }
  }

  return false;
}

