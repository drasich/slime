#ifndef __vec__
#define __vec__

typedef struct _Vec3 Vec3;

struct _Vec3 {
  double X;
  double Y;
  double Z;
};

typedef struct _Vec4 Vec4;

struct _Vec4 {
  double X;
  double Y;
  double Z;
  double W;
};

Vec3 vec3_zero();

double vec3_length(Vec3 v);
double vec4_length(Vec4 v);
double vec4_length2(Vec4 v);

Vec3 vec3_add(Vec3 v1, Vec3 v2);
Vec3 vec3_sub(Vec3 v1, Vec3 v2);
Vec3 vec3_mul(Vec3 v1, double s);

double vec3_dot(Vec3 v1, Vec3 v2);

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


typedef struct _IntersectionRayPlane IntersectionRayPlane;

struct _IntersectionRayPlane {
  short hit;
  Vec3 position;
};

IntersectionRayPlane intersection_ray_plane(Ray ray, Plane plane);

typedef struct _AABox AABox;

struct _AABox {
  Vec3 Min;
  Vec3 Max;
};

//TODO Intersection ray - aaabox

#endif
