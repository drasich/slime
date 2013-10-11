#ifndef __matrix__
#define __matrix__
#include "gl.h"
#include "quat.h"
#undef near
#undef far

// matrices are row major in this code:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15
// while opengL is column major:
// 0 4 8 12
// 1 5 9 13
// 2 6 10 14
// 3 7 11 15
// so we need to transpose before sending

typedef double Matrix4[16];
typedef double Matrix3[9];
typedef GLfloat Matrix4GL[16];
typedef GLfloat Matrix3GL[9];

void mat4_to_mat3(const Matrix4 in, Matrix3 out);
void mat3_inverse(const Matrix3 in, Matrix3 out);
void mat4_inverse(const Matrix4 in, Matrix4 out);

void mat3_set_identity(Matrix3 m);
void mat4_set_identity(Matrix4 m);

void mat3_to_gl(const Matrix3 in, Matrix3GL out);
void mat4_to_gl(const Matrix4 in, Matrix4GL out);

void mat4_set_frustum(
      Matrix4 m,
      double left,
      double right,
      double bottom,
      double top,
      double near,
      double far);

void mat4_set_perspective(
      Matrix4 m,
      double fovy,
      double aspect,
      double near,
      double far);

void mat4_set_orthographic(
      Matrix4 m,
      uint16_t half_width,
      uint16_t half_height,
      double near,
      double far);


void mat4_multiply(const Matrix4 m, const Matrix4 n, Matrix4 out);

void mat4_transpose(const Matrix4 in, Matrix4 out);
void mat3_transpose(const Matrix4 in, Matrix4 out);

void mat4_set_translation(Matrix4 m, Vec3);
void mat4_pre_translate(Matrix4 m, Vec3);

void mat4_zero(Matrix4 m);
void mat3_zero(Matrix4 m);

void mat4_set_rotation_quat(Matrix4 m, Quat q);

Vec3 mat4_mul(const Matrix4 m, Vec3 v);
Vec3 mat4_premul(const Matrix4 m, Vec3 v);

Vec4 mat4_vec4_mul(const Matrix4 m, Vec4 v);
Vec4 mat4_vec4_premul(const Matrix4 m, Vec4 v);

void mat4_pos_ori(Vec3 position, Quat orientation, Matrix4 out);

void mat4_lookat(Matrix4 m, Vec3 position, Vec3 at, Vec3 up);

void mat4_decompose(Matrix4 m, Vec3* position, Quat* rotation, Vec3* scale);

Quat mat4_get_quat(Matrix4 m);

void mat4_set_scale(Matrix4 m, const Vec3 v);

void mat4_copy(const Matrix4 in, Matrix4 out);

#endif


