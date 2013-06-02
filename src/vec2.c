#include "vec2.h"
#include <math.h>

Vec2
vec2(double x, double y)
{
  Vec2 v = {
    .X = x,
    .Y = y
  };

  return v;
}

double
vec2_length(Vec2 v)
{
  return sqrt(v.X*v.X + v.Y*v.Y);
}

double
vec2_length2(Vec2 v)
{
  return v.X*v.X + v.Y*v.Y;
}

Vec2
vec2_add(Vec2 v1, Vec2 v2)
{
  Vec2 v;
  v.X = v1.X + v2.X;
  v.Y = v1.Y + v2.Y;
  return v;
}

Vec2
vec2_sub(Vec2 v1, Vec2 v2)
{
  Vec2 v;
  v.X = v1.X - v2.X;
  v.Y = v1.Y - v2.Y;
  return v;
}

Vec2
vec2_mul(Vec2 v1, double s)
{
  Vec2 v;
  v.X = v1.X*s;
  v.Y = v1.Y*s;
  return v;
}


