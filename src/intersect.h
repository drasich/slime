#ifndef __intersect__
#define __intersect__
#include "vec.h"
#include "quat.h"
#include <stdbool.h>

typedef struct _Ray Ray;

struct _Ray {
  Vec3 Start;
  Vec3 Direction;
};

typedef struct _Plane Plane;

struct _Plane {
  Vec3 Point;
  Vec3 Normal;
};

typedef struct _Sphere Sphere;
struct _Sphere {
  Vec3 center;
  float radius;
};

typedef struct _IntersectionRay IntersectionRay;

struct _IntersectionRay {
  bool hit;
  bool inside;
  Vec3 position;
  Vec3 normal;
};

IntersectionRay intersection_ray_plane(Ray ray, Plane plane);
IntersectionRay intersection_ray_sphere(Ray ray, Sphere sphere);

typedef struct _AABox AABox;
struct _AABox {
  Vec3 Min;
  Vec3 Max;
};

//TODO Intersection ray - aaabox
IntersectionRay intersection_ray_aabox(Ray ray, AABox box);
IntersectionRay intersection_ray_box(Ray ray, AABox box, Vec3 position, Quat rotation);

typedef struct _Repere Repere;
struct _Repere {
  Vec3 origin;
  Quat rotation;
};

Vec3 world_to_local(Repere r, Vec3 v);
Vec3 local_to_world(Repere r, Vec3 v);

typedef struct _Triangle Triangle;
struct _Triangle {
  Vec3 v0;
  Vec3 v1;
  Vec3 v2;
};

IntersectionRay intersection_ray_triangle(Ray ray, Triangle, double min);

#endif
