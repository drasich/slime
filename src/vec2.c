#include "vec2.h"
#include <math.h>

Vec2
vec2(double x, double y)
{
  Vec2 v = {
    .x = x,
    .y = y
  };

  return v;
}

double
vec2_length(Vec2 v)
{
  return sqrt(v.x*v.x + v.y*v.y);
}

double
vec2_length2(Vec2 v)
{
  return v.x*v.x + v.y*v.y;
}

Vec2
vec2_add(Vec2 v1, Vec2 v2)
{
  Vec2 v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

Vec2
vec2_sub(Vec2 v1, Vec2 v2)
{
  Vec2 v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  return v;
}

Vec2
vec2_mul(Vec2 v1, double s)
{
  Vec2 v;
  v.x = v1.x*s;
  v.y = v1.y*s;
  return v;
}

Vec2
vec2_normalized(Vec2 v)
{
  Vec2 r = vec2_mul(v, 1.0f/vec2_length(v));
  return r;
}

double
vec2_dot(Vec2 v1, Vec2 v2)
{
  return v1.x*v2.x + v1.y*v2.y;
}

