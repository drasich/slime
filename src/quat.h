#ifndef __quat__
#define __quat__
#include "vec.h"
#include <math.h>

typedef Vec4 Quat;

Quat quat_identity();
Quat quat_angle_axis(double angle, Vec3 axis);
void quat_set_identity(Quat* q);

double quat_length2(Quat v);

#endif
