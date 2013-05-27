#ifndef __geometry__
#define __geometry__
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

typedef struct _Repere Repere;
struct _Repere {
  Vec3 origin;
  Quat rotation;
};

typedef struct _Triangle Triangle;
struct _Triangle {
  Vec3 v0;
  Vec3 v1;
  Vec3 v2;
};


typedef struct _Frustum Frustum;
struct _Frustum {
  float near;
  float far;
  Vec3 start;
  Vec3 direction;
  Vec3 up;
  float fovy;
  float aspect;
};

void frustum_set(
      Frustum* f, 
      float near, 
      float far, 
      Vec3 start, 
      Vec3 direction,
      Vec3 up,
      float fovy,
      float aspect);

typedef struct _AABox AABox;
struct _AABox {
  Vec3 Min;
  Vec3 Max;
};



#endif
