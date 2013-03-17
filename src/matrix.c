#include "matrix.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

void
mat4_to_mat3(const Matrix4 in, Matrix3 out)
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
mat3_inverse(const Matrix3 in, Matrix3 out)
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
mat3_to_gl(const Matrix3 in, Matrix3GL out)
{
  int i;
  for (i = 0; i < 9; ++i) {
    out[i] = (GLfloat) in[i];
  }
}

void
mat4_to_gl(const Matrix4 in, Matrix4GL out)
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

  m[0] = 2 * near / (right - left);
  m[2] = (right + left) / (right - left);
  m[5] = 2 * near / (top - bottom);
  m[6] = (top + bottom) / (top - bottom);
  m[10] = -(far + near) / (far - near);
  m[11] = -(2 * far * near) / (far - near);
  m[14] = -1;
  m[15] = 0;
}

void
mat4_set_perspective(
      Matrix4 m,
      double fovy,
      double aspect,
      double near,
      double far)
{
  double half_height = tan(fovy/2.0)*near;
  double half_width = half_height* aspect;

  mat4_set_frustum(m, -half_width, half_width, -half_height, half_height, near, far);
}

void
mat4_set_orthographic(
      Matrix4 m,
      uint16_t hw,
      uint16_t hh,
      double near,
      double far)
{
  m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[12] = m[13] = m[14] = 0;

  m[0] =  1/(double) hw;
  m[5] =  1/(double) hh;
  m[10] = -2 / (far - near);
  m[11] = - (far + near) / (far - near);
  m[15] = 1;
}


void
mat4_multiply(const Matrix4 m, const Matrix4 n, Matrix4 out)
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
mat3_transpose(const Matrix3 in, Matrix3 out)
{
  double tmp[9];

  tmp[0] = in[0];
  tmp[1] = in[3];
  tmp[2] = in[6];

  tmp[3] = in[1];
  tmp[4] = in[4];
  tmp[5] = in[7];

  tmp[6] = in[2];
  tmp[7] = in[5];
  tmp[8] = in[8];

  int i;
  for (i = 0; i < 9; ++i) {
    out[i] = tmp[i];
  }
}


void
mat4_transpose(const Matrix4 in, Matrix4 out)
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

static double
_cofactor(
      double m0,
      double m1,
      double m2,
      double m3,
      double m4,
      double m5,
      double m6,
      double m7,
      double m8
      )
{
  return 
   m0 * (m4 * m8 - m5 * m7) -
   m1 * (m3 * m8 - m5 * m6) +
   m2 * (m3 * m7 - m4 * m6);
}
static void
_mat4_inverse_affine(const Matrix4 m, Matrix4 out)
{
  // R^-1
  //Matrix3 r = {m[0],m[1],m[2], m[4],m[5],m[6], m[8],m[9],m[10]};
  Matrix3 r;
  mat4_to_mat3(m,r);
  mat3_inverse(r,r);

  out[0] = r[0];  out[1] = r[1];  out[2] = r[2];
  out[4] = r[3];  out[5] = r[4];  out[6] = r[5];
  out[8] = r[6];  out[9] = r[7];  out[10]= r[8];

  // -R^-1 * T
  double x = m[3];
  double y = m[7];
  double z = m[11];
  out[3]  = -(r[0] * x + r[1] * y + r[2] * z);
  out[7]  = -(r[3] * x + r[4] * y + r[5] * z);
  out[11] = -(r[6] * x + r[7] * y + r[8] * z);

  // last row should be unchanged (0,0,0,1)
  out[12] = out[13] = out[14] = 0.0f;
  out[15] = 1.0f;
}

static void
_mat4_inverse_general(const Matrix4 m, Matrix4 out)
{
  // get cofactors of minor matrices
  double cofactor0 = _cofactor(m[5],m[6],m[7], m[9],m[10],m[11], m[13],m[14],m[15]);
  double cofactor1 = _cofactor(m[4],m[6],m[7], m[8],m[10],m[11], m[12],m[14],m[15]);
  double cofactor2 = _cofactor(m[4],m[5],m[7], m[8],m[9], m[11], m[12],m[13],m[15]);
  double cofactor3 = _cofactor(m[4],m[5],m[6], m[8],m[9], m[10], m[12],m[13],m[14]);

  // get determinant
  double determinant = m[0] * cofactor0 - m[1] * cofactor1 + m[2] * cofactor2 - m[3] * cofactor3;
  if(fabs(determinant) <= 0.00001f) {
    mat4_set_identity(out);
   }

  // get rest of cofactors for adj(M)
  double cofactor4 = _cofactor(m[1],m[2],m[3], m[9],m[10],m[11], m[13],m[14],m[15]);
  double cofactor5 = _cofactor(m[0],m[2],m[3], m[8],m[10],m[11], m[12],m[14],m[15]);
  double cofactor6 = _cofactor(m[0],m[1],m[3], m[8],m[9], m[11], m[12],m[13],m[15]);
  double cofactor7 = _cofactor(m[0],m[1],m[2], m[8],m[9], m[10], m[12],m[13],m[14]);

  double cofactor8 = _cofactor(m[1],m[2],m[3], m[5],m[6], m[7],  m[13],m[14],m[15]);
  double cofactor9 = _cofactor(m[0],m[2],m[3], m[4],m[6], m[7],  m[12],m[14],m[15]);
  double cofactor10= _cofactor(m[0],m[1],m[3], m[4],m[5], m[7],  m[12],m[13],m[15]);
  double cofactor11= _cofactor(m[0],m[1],m[2], m[4],m[5], m[6],  m[12],m[13],m[14]);

  double cofactor12= _cofactor(m[1],m[2],m[3], m[5],m[6], m[7],  m[9], m[10],m[11]);
  double cofactor13= _cofactor(m[0],m[2],m[3], m[4],m[6], m[7],  m[8], m[10],m[11]);
  double cofactor14= _cofactor(m[0],m[1],m[3], m[4],m[5], m[7],  m[8], m[9], m[11]);
  double cofactor15= _cofactor(m[0],m[1],m[2], m[4],m[5], m[6],  m[8], m[9], m[10]);

  // build inverse matrix = adj(M) / det(M)
  // adjugate of M is the transpose of the cofactor matrix of M
  double invDeterminant = 1.0f / determinant;
  out[0] =  invDeterminant * cofactor0;
  out[1] = -invDeterminant * cofactor4;
  out[2] =  invDeterminant * cofactor8;
  out[3] = -invDeterminant * cofactor12;

  out[4] = -invDeterminant * cofactor1;
  out[5] =  invDeterminant * cofactor5;
  out[6] = -invDeterminant * cofactor9;
  out[7] =  invDeterminant * cofactor13;

  out[8] =  invDeterminant * cofactor2;
  out[9] = -invDeterminant * cofactor6;
  out[10]=  invDeterminant * cofactor10;
  out[11]= -invDeterminant * cofactor14;

  out[12]= -invDeterminant * cofactor3;
  out[13]=  invDeterminant * cofactor7;
  out[14]= -invDeterminant * cofactor11;
  out[15]=  invDeterminant * cofactor15;
}

void
mat4_inverse(const Matrix4 m, Matrix4 out)
{
  if(m[12] == 0 && m[13] == 0 && m[14] == 0 && m[15] == 1)
  _mat4_inverse_affine(m, out);
  else
  _mat4_inverse_general(m, out);
}


Vec3 
mat4_mul(const Matrix4 m, Vec3 v)
{
  return vec3(
        m[0]*v.X + m[1]*v.Y + m[2]*v.Z,
        m[4]*v.X + m[5]*v.Y + m[6]*v.Z,
        m[8]*v.X + m[9]*v.Y + m[10]*v.Z
        );
}

Vec3
mat4_premul(const Matrix4 m, Vec3 v)
{
  return vec3(
        v.X*m[0] + v.Y*m[4] + v.Z*m[8],
        v.X*m[1] + v.Y*m[5] + v.Z*m[9],
        v.X*m[2] + v.Y*m[6] + v.Z*m[10]);
}

void
mat4_pos_ori(Vec3 position, Quat orientation, Matrix4 out)
{
  Matrix4 mt, mr;
  mat4_set_translation(mt, position);
  mat4_set_rotation_quat(mr, orientation);
  mat4_multiply(mt, mr, out);
}

void 
mat4_lookat(Matrix4 m, Vec3 position, Vec3 at, Vec3 up)
{
  Vec3 d = vec3_sub(at, position);
  d = vec3_normalized(d);
  Vec3 s = vec3_cross(d, up);
  s = vec3_normalized(s);
  Vec3 u = vec3_cross(s, d);
  u = vec3_normalized(u);

  m[0] = s.X;
  m[1] = u.X;
  m[2] = -d.X;
  m[3] = 0.0;

  m[4] = s.Y;
  m[5] = u.Y;
  m[6] = -d.Y;
  m[7] = 0.0;

  m[8] = s.Z;
  m[9] = u.Z;
  m[10] = -d.Z;
  m[11] = 0.0;

  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;

  //mat4_pre_translate(m, vec3_mul(position, -1));
}

void 
mat4_pre_translate(Matrix4 m, Vec3 v)
{
  double tmp = v.X;
  if (tmp != 0) {
    m[12] += tmp*m[0];
    m[13] += tmp*m[1];
    m[14] += tmp*m[2];
    m[15] += tmp*m[3];
  }

  tmp = v.Y;
  if (tmp != 0) {
    m[12] += tmp*m[4];
    m[13] += tmp*m[5];
    m[14] += tmp*m[6];
    m[15] += tmp*m[7];
  }

  tmp = v.Z;
  if (tmp != 0) {
    m[12] += tmp*m[8];
    m[13] += tmp*m[9];
    m[14] += tmp*m[10];
    m[15] += tmp*m[11];
  }
}

typedef struct
{
  Vec4 t;
  Quat q;
  Quat u;
  Quat qk;
  double f;
} _AffineParts;

void 
mat4_decomp_affine(Matrix4 hm, _AffineParts* parts)
{
  Matrix4 Q, S, U;
  Quat p;

  parts->t = vec4(hm[3], hm[7], hm[11], 0.0);
  //double det = 

}

void 
mat4_decompose(Matrix4 m, Vec3* position, Quat* rotation, Vec3* scale)
{
  Matrix4 hm;
  mat4_transpose(m, hm);

  _AffineParts parts;
  mat4_decomp_affine(hm, &parts);
}


Quat
mat4_get_quat_sav(Matrix4 m)
{
  Quat q;

  double s;
  double tq[4];
  int i, j;

  tq[0] = 1 + m[0] + m[5] + m[10];
  tq[1] = 1 + m[0] - m[5] - m[10];
  tq[2] = 1 - m[0] + m[5] - m[10];
  tq[3] = 1 - m[0] - m[5] + m[10];

  j = 0;
  for (i = 1; i < 4; ++i) {
    j = (tq[i] > tq[j]) ? i : j;
  }

  if (j == 0) {
    q.W = tq[0];
    q.X = m[6] - m[9];
    q.Y = m[8] - m[2];
    q.Z = m[1] - m[4];
  } else if (j == 1) {
    q.W = m[6] - m[9];
    q.X = tq[1];
    q.Y = m[1] + m[4];
    q.Z = m[8] + m[2];
  } else if (j == 2) {
    q.W = m[8] - m[2];
    q.X = m[1] + m[4];
    q.Y = tq[2];
    q.Z = m[6] + m[9];
  } else {
    q.W = m[1] - m[4];
    q.X = m[8] + m[2];
    q.Y = m[6] + m[9];
    q.Z = tq[3];
  }

  s = sqrt(0.25/tq[j]);
  q.W *= s;
  q.X *= s;
  q.Y *= s;
  q.Z *= s;

  return q;

}


Quat
mat4_get_quat(Matrix4 m)
{
  Quat q;

  double t = 1 + m[0] + m[5] + m[10];
  double s;

   if (t > 0.00000001) {
     s = sqrt(t) * 2;
     q.X = ( m[9] - m[6] ) / s;
     q.Y = ( m[2] - m[8] ) / s;
     q.Z = ( m[4] - m[1] ) / s;
     q.W = 0.25 * s;
  } else if (t < 0) {
    printf("c'est plus petit que 0 \n");
  } else {
    if ( m[0] > m[5] && m[0] > m[10] )  {	// Column 0: 
      s  = sqrt( 1.0 + m[0] - m[5] - m[10] ) * 2;
      q.X = 0.25 * s;
      q.Y = (m[4] + m[1] ) / s;
      q.Z = (m[2] + m[8] ) / s;
      q.W = (m[9] - m[6] ) / s;
    } else if ( m[5] > m[10] ) {			// Column 1: 
      s  = sqrt( 1.0 + m[5] - m[0] - m[10] ) * 2;
      q.X = (m[4] + m[1] ) / s;
      q.Y = 0.25 * s;
      q.Z = (m[9] + m[6] ) / s;
      q.W = (m[2] - m[8] ) / s;
    } else {						// Column 2:
      s  = sqrt( 1.0 + m[10] - m[0] - m[5] ) * 2;
      q.X = (m[2] + m[8] ) / s;
      q.Y = (m[9] + m[6] ) / s;
      q.Z = 0.25 * s;
      q.W = (m[4] - m[1] ) / s;
    }
  }

   return q;
}
