#include "quat.h"

double
quat_length2(Quat v)
{
  return v.X*v.X + v.Y*v.Y + v.Z*v.Z + v.W*v.W;
}

const double epsilon = 0.0000001;

Quat
quat_identity()
{
  Quat q = { .X = 0, .Y = 0, .Z = 0, .W = 1 };
  return q;
}

void
quat_set_identity(Quat* q)
{
  q->X = q->Y = q->Z = 0;
  q->W = 1;
}


Quat
quat_angle_axis(double angle, Vec3 axis)
{
  double length = vec3_length(axis);
  if (length < epsilon) {
    return quat_identity();
  }

  double inverse_norm = 1/length;
  double cos_half_angle = cos(0.5f*angle);
  double sin_half_angle = sin(0.5f*angle);

  Quat q = { 
    .X = axis.X * sin_half_angle * inverse_norm,
    .Y = axis.Y * sin_half_angle * inverse_norm,
    .Z = axis.Z * sin_half_angle * inverse_norm,
    .W = cos_half_angle};

  return q;
}


