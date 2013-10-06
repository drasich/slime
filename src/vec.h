#ifndef __vec__
#define __vec__
#include "vec2.h"
#include <stdbool.h>

//#define PRINT_VEC3 
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

double vec3_dot(Vec3 v1, Vec3 v2);
Vec3 vec3_cross(Vec3 v1, Vec3 v2);

double vec4_dot(Vec4 v1, Vec4 v2);

bool vec3_equal(Vec3 v1, Vec3 v2);
bool vec4_equal(Vec4 v1, Vec4 v2);


#endif
