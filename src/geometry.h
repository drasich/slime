#ifndef __geometry__
#define __geometry__
#include "vec.h"
#include "quat.h"
#include <stdbool.h>
#undef near
#undef far

typedef struct _Ray Ray;
struct _Ray {
  Vec3 start;
  Vec3 direction;
};

typedef struct _Plane Plane;
struct _Plane {
  Vec3 point;
  Vec3 normal;
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
  Vec3 min;
  Vec3 max;
};


typedef Vec3 OBox[8];
/*
typedef struct _OBox OBox;
struct _OBox {
  Vec3 MinX;
  Vec3 MaxX;
  Vec3 MinY;
  Vec3 MaxY;
  Vec3 MinZ;
  Vec3 MaxZ;
};
*/

void aabox_to_obox(AABox a, OBox o, Vec3 v, Quat q, Vec3 scale);


typedef struct _Segment Segment;

struct _Segment {
  Vec3 p0;
  Vec3 p1;
};



#endif
