#ifndef __vec__
#define __vec__
#include "vec2.h"
#include <stdbool.h>

//#define PRINT_VEC3 
typedef struct _Vec3 Vec3;
typedef struct _Vec3f Vec3f;

struct _Vec3 {
  double x;
  double y;
  double z;
};

struct _Vec3f {
  float x;
  float y;
  float z;
};

typedef struct _Vec4 Vec4;
typedef struct _Vec4f Vec4f;

struct _Vec4 {
  double x;
  double y;
  double z;
  double w;
};

struct _Vec4f {
  float x;
  float y;
  float z;
  float w;
};


Vec3 vec3_zero();

Vec3 vec3(double x, double y, double z);
Vec4 vec4(double x, double y, double z, double w);

double vec3_length(Vec3 v);
double vec3_length2(Vec3 v);
double vec4_length(Vec4 v);
double vec4_length2(Vec4 v);

Vec3 vec3_add(Vec3 v1, Vec3 v2);
Vec3 vec3_sub(Vec3 v1, Vec3 v2);
Vec3 vec3_mul(Vec3 v1, double s);
Vec3 vec3_vec3_mul(Vec3 v1, Vec3 v2);
Vec3 vec3_normalized(Vec3 v);
Vec3f vec3d_to_vec3f(Vec3 v);

double vec3_dot(Vec3 v1, Vec3 v2);
Vec3 vec3_cross(Vec3 v1, Vec3 v2);

double vec4_dot(Vec4 v1, Vec4 v2);

bool vec3_equal(Vec3 v1, Vec3 v2);
bool vec4_equal(Vec4 v1, Vec4 v2);


#endif
