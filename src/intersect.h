#ifndef __intersect__
#define __intersect__
#include "vec.h"
#include "quat.h"
#include "object.h"
#include "geometry.h"
#include <stdbool.h>

typedef struct _IntersectionRay IntersectionRay;

struct _IntersectionRay {
  bool hit;
  bool inside;
  Vec3 position;
  Vec3 normal;
};

IntersectionRay intersection_ray_plane(Ray ray, Plane plane);
IntersectionRay intersection_ray_sphere(Ray ray, Sphere sphere);

//TODO Intersection ray - aaabox
IntersectionRay intersection_ray_aabox(Ray ray, AABox box);
IntersectionRay intersection_ray_box(Ray ray, AABox box, Vec3 position, Quat rotation);

Vec3 world_to_local(Repere r, Vec3 v);
Vec3 local_to_world(Repere r, Vec3 v);

IntersectionRay intersection_ray_triangle(Ray ray, Triangle, double min);
IntersectionRay intersection_ray_object(Ray ray, Object* o);


bool frustum_is_in(Frustum* f, Vec3 p);
bool frustum_is_in_rect(Frustum* f, Vec3 p, float left, float top, float width, float height);

bool planes_is_in(Plane*, int nb_planes, Vec3 p);

#endif
