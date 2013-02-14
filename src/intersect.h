#ifndef __intersect__
#define __intersect__
#include "vec.h"

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
  short hit;
  Vec3 position;
};

IntersectionRay intersection_ray_plane(Ray ray, Plane plane);
IntersectionRay intersection_ray_sphere(Ray ray, Sphere sphere);

typedef struct _AABox AABox;

struct _AABox {
  Vec3 Min;
  Vec3 Max;
};

//TODO Intersection ray - aaabox


#endif
