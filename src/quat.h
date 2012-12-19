#ifndef __quat__
#define __quat__
#include "vec.h"

typedef Vec4 Quat;


Quat quat_identity();
Quat quat_angle_axis(double angle, Vec3 axis);

double quat_length2(Quat v);

#endif
