#ifndef __quat__
#define __quat__
#include "vec.h"
#include <math.h>

typedef Vec4 Quat;

Quat quat_identity();
Quat quat_angle_axis(double angle, Vec3 axis);
void quat_set_identity(Quat* q);
Quat quat_yaw_pitch_roll_rad(double yaw, double pitch, double roll);
Quat quat_yaw_pitch_roll_deg(double yaw, double pitch, double roll);
Quat quat_angles_deg(Vec3 angles);
Quat quat_angles_rad(Vec3 angles);

double quat_length2(Quat v);
Quat quat_mul(Quat ql,Quat qr);
Quat quat_mul_scalar(Quat q, float s);
Vec3 quat_rotate_vec3(Quat q, Vec3 v);
Quat quat_add(Quat ql, Quat qr);

Quat quat_conj(Quat q);
Quat quat_inverse(Quat q);
Quat quat_between_quat(Quat q1, Quat q2);
Quat quat_between_vec(Vec3 from, Vec3 to);

Quat quat_slerp(Quat from, Quat to, float t);

Vec4 quat_to_axis_angle(Quat q);

Vec3 quat_rotate_around_angles(Vec3 pivot, Vec3 mypoint, float yaw, float pitch);
Vec3 quat_rotate_around(Quat q, Vec3 pivot, Vec3 mypoint);


Quat quat_lookat(Vec3 from, Vec3 to, Vec3 up);

Vec3 quat_to_euler(Quat q);
Vec3 quat_to_euler_deg(Quat q);

bool quat_equal(Quat q1, Quat q2);

#endif
