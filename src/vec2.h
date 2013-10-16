#ifndef __vec2__
#define __vec2__
#include <stdbool.h>

typedef struct _Vec2 Vec2;

struct _Vec2 {
  double x;
  double y;
};

Vec2 vec2(double x, double y);

double vec2_length(Vec2 v);
double vec2_length2(Vec2 v);

Vec2 vec2_add(Vec2 v1, Vec2 v2);
Vec2 vec2_sub(Vec2 v1, Vec2 v2);
Vec2 vec2_mul(Vec2 v1, double s);
Vec2 vec2_normalized(Vec2 v);
double vec2_dot(Vec2 v1, Vec2 v2);

#endif
