#ifndef __quat__
#define __quat__
#include "vec.h"
#include <math.h>

typedef Vec4 Quat;

Quat quat_identity();
Quat quat_angle_axis(double angle, Vec3 axis);
void quat_set_identity(Quat* q);

double quat_length2(Quat v);
Quat quat_mul(Quat ql, Quat qr);
Quat quat_mul_scalar(Quat q, float s);
Vec3 quat_rotate_vec3(Quat q, Vec3 v);

Quat quat_conj(Quat q);
Quat quat_inverse(Quat q);
Quat quat_from_quat_to_quat(Quat q1, Quat q2);

#endif