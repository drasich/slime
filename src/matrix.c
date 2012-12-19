#include "matrix.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

void
mat4_to_mat3(Matrix4 in, Matrix3 out)
{
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];

  out[3] = in[4];
  out[4] = in[5];
  out[5] = in[6];

  out[6] = in[8];
  out[7] = in[9];
  out[8] = in[10];
}

void
mat3_inverse(Matrix3 in, Matrix3 out)
{
  double determinant, inv_determinant;
  double tmp[9];

  tmp[0] = in[4] * in[8] - in[5] * in[7];
  tmp[1] = in[2] * in[7] - in[1] * in[8];
  tmp[2] = in[1] * in[5] - in[2] * in[4];
  tmp[3] = in[5] * in[6] - in[3] * in[8];
  tmp[4] = in[0] * in[8] - in[2] * in[6];
  tmp[5] = in[2] * in[3] - in[0] * in[5];
  tmp[6] = in[3] * in[7] - in[4] * in[6];
  tmp[7] = in[1] * in[6] - in[0] * in[7];
  tmp[8] = in[0] * in[4] - in[1] * in[3];

  determinant = in[0] * tmp[0] + in[1] * tmp[3] + in[2] * tmp[6];

  if (fabs(determinant) <= DBL_MIN) {
    mat3_set_identity(out);
    return;
  }

  inv_determinant = 1.0f / determinant;
  out[0] = inv_determinant * tmp[0];
  out[1] = inv_determinant * tmp[1];
  out[2] = inv_determinant * tmp[2];
  out[3] = inv_determinant * tmp[3];
  out[4] = inv_determinant * tmp[4];
  out[5] = inv_determinant * tmp[5];
  out[6] = inv_determinant * tmp[6];
  out[7] = inv_determinant * tmp[7];
  out[8] = inv_determinant * tmp[8];
}

void
mat3_set_identity(Matrix3 m)
{
  m[0] = 1;
  m[4] = 1;
  m[8] = 1;
  m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0;
}

void
mat4_set_identity(Matrix4 m)
{
  m[0] = 1;
  m[5] = 1;
  m[10] = 1;
  m[15] = 1;
  m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = 0;
  m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0;
}

void
mat3_to_gl(Matrix3 in, Matrix3GL out)
{
  int i;
  for (i = 0; i < 9; ++i) {
    out[i] = (GLfloat) in[i];
  }
}

void
mat4_to_gl(Matrix4 in, Matrix4GL out)
{
  int i;
  for (i = 0; i < 16; ++i) {
    out[i] = (GLfloat) in[i];
  }
}

void 
mat4_set_frustum(
      Matrix4 m,
      double left,
      double right,
      double bottom,
      double top,
      double near,
      double far)
{
  m[1] = m[3] = m[4] = m[7] = m[8] = m[9] = m[12] = m[13] = 0;

  m[0]  =  2 * near / (right - left);
  m[2]  =  (right + left) / (right - left);
  m[5]  =  2 * near / (top - bottom);
  m[6]  =  (top + bottom) / (top - bottom);
  m[10] = -(far + near) / (far - near);
  m[11] = -(2 * far * near) / (far - near);
  m[14] = -1;
  m[15] =  0;
}


void
mat4_multiply(Matrix4 m, Matrix4 n, Matrix4 out)
{
  // we use a tmp in case out is the same as m or n.
  // we could make an assert to make sure they are different pointers...
  double tmp[16];
  tmp[0] = m[0]*n[0] + m[1]*n[4] + m[2]*n[8] + m[3]*n[12];
  tmp[1] = m[0]*n[1] + m[1]*n[5] + m[2]*n[9] + m[3]*n[13];
  tmp[2] = m[0]*n[2] + m[1]*n[6] + m[2]*n[10] + m[3]*n[14];
  tmp[3] = m[0]*n[3] + m[1]*n[7] + m[2]*n[11] + m[3]*n[15];
  tmp[4] = m[4]*n[0] + m[5]*n[4] + m[6]*n[8] + m[7]*n[12];
  tmp[5] = m[4]*n[1] + m[5]*n[5] + m[6]*n[9] + m[7]*n[13];
  tmp[6] = m[4]*n[2] + m[5]*n[6] + m[6]*n[10] + m[7]*n[14];
  tmp[7] = m[4]*n[3] + m[5]*n[7] + m[6]*n[11] + m[7]*n[15];
  tmp[8] = m[8]*n[0] + m[9]*n[4] + m[10]*n[8] + m[11]*n[12];
  tmp[9] = m[8]*n[1] + m[9]*n[5] + m[10]*n[9] + m[11]*n[13];
  tmp[10] = m[8]*n[2] + m[9]*n[6] + m[10]*n[10] + m[11]*n[14];
  tmp[11] = m[8]*n[3] + m[9]*n[7] + m[10]*n[11] + m[11]*n[15];
  tmp[12] = m[12]*n[0] + m[13]*n[4] + m[14]*n[8] + m[15]*n[12];
  tmp[13] = m[12]*n[1] + m[13]*n[5] + m[14]*n[9] + m[15]*n[13];
  tmp[14] = m[12]*n[2] + m[13]*n[6] + m[14]*n[10] + m[15]*n[14];
  tmp[15] = m[12]*n[3] + m[13]*n[7] + m[14]*n[11] + m[15]*n[15];

  int i;
  for (i = 0; i < 16; ++i) {
    out[i] = tmp[i];
  }
}

void
mat4_transpose(Matrix4 in, Matrix4 out)
{
  double tmp[16];

  tmp[0] = in[0];
  tmp[1] = in[4];
  tmp[2] = in[8];
  tmp[3] = in[12];

  tmp[4] = in[1];
  tmp[5] = in[5];
  tmp[6] = in[9];
  tmp[7] = in[13];

  tmp[8] = in[2];
  tmp[9] = in[6];
  tmp[10]= in[10];
  tmp[11]= in[14];

  tmp[12]= in[3];
  tmp[13]= in[7];
  tmp[14]= in[11];
  tmp[15]= in[15];

  int i;
  for (i = 0; i < 16; ++i) {
    out[i] = tmp[i];
  }
}

void
mat4_set_translation(Matrix4 m, Vec3 t)
{
  m[0] = m[5] = m[10] = m[15] = 1;
  m[3] = t.X;
  m[7] = t.Y;
  m[11] = t.Z;

  m[1] = m[2] = m[4] = 0;
  m[6] = m[8] = m[9] = 0;
  m[12] = m[13] = m[14] = 0;
}

void
mat4_zero(Matrix4 m)
{
  int i;
  for (i = 0; i < 16; ++i) m[i] = 0;
}

void
mat3_zero(Matrix3 m)
{
  int i;
  for (i = 0; i < 9; ++i) m[i] = 0;
}

void 
mat4_set_rotation_quat(Matrix4 m, Quat q)
{
  double length2 = quat_length2(q);
  if (fabs(length2) <= DBL_MIN) {
    mat4_set_identity(m);
    return;
  }

  double rlength2;
  if (length2 != 1) rlength2 = 2.0f/length2;
  else rlength2 = 2;

  double x2, y2, z2, xx, xy, xz, yy, yz, zz, wx, wy, wz;

  x2 = rlength2*q.X;
  y2 = rlength2*q.Y;
  z2 = rlength2*q.Z;

  xx = q.X * x2;
  xy = q.X * y2;
  xz = q.X * z2;

  yy = q.Y * y2;
  yz = q.Y * z2;
  zz = q.Z * z2;

  wx = q.W * x2;
  wy = q.W * y2;
  wz = q.W * z2;

  m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0;

  m[15] = 1;

  m[0] = 1 - (yy + zz);
  m[4] = xy - wz;
  m[8] = xz + wy;

  m[1] = xy + wz;
  m[5] = 1 - (xx + zz);
  m[9] = yz - wx;

  m[2] = xz - wy;
  m[6] = yz + wx;
  m[10] = 1 - (xx + yy);
}

