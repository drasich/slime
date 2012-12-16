#include "matrix.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

Matrix3
mat4_to_mat3(Matrix4 mat)
{
  Matrix3 m;
  m.m[0] = mat.m[0];
  m.m[1] = mat.m[1];
  m.m[2] = mat.m[2];

  m.m[3] = mat.m[4];
  m.m[4] = mat.m[5];
  m.m[5] = mat.m[6];

  m.m[6] = mat.m[8];
  m.m[7] = mat.m[9];
  m.m[8] = mat.m[10];
  return m;
}

Matrix3
mat3_inverse(Matrix3 m)
{
  Matrix3 tm;
  double determinant, inv_determinant;
  double tmp[9];

  tmp[0] = m.m[4] * m.m[8] - m.m[5] * m.m[7];
  tmp[1] = m.m[2] * m.m[7] - m.m[1] * m.m[8];
  tmp[2] = m.m[1] * m.m[5] - m.m[2] * m.m[4];
  tmp[3] = m.m[5] * m.m[6] - m.m[3] * m.m[8];
  tmp[4] = m.m[0] * m.m[8] - m.m[2] * m.m[6];
  tmp[5] = m.m[2] * m.m[3] - m.m[0] * m.m[5];
  tmp[6] = m.m[3] * m.m[7] - m.m[4] * m.m[6];
  tmp[7] = m.m[1] * m.m[6] - m.m[0] * m.m[7];
  tmp[8] = m.m[0] * m.m[4] - m.m[1] * m.m[3];

  determinant = m.m[0] * tmp[0] + m.m[1] * tmp[3] + m.m[2] * tmp[6];

  if ( fabs(determinant) <= DBL_MIN )
  return mat3_identity();
  return tm;
}

void
mat3_set_identity(Matrix3* m)
{
  m->m[0] = 1;
  m->m[4] = 1;
  m->m[8] = 1;
  m->m[1] = m->m[2] = m->m[3] = m->m[5] = m->m[6] = m->m[7] = 0;
}

Matrix3
mat3_identity()
{
  Matrix3 m;
  mat3_set_identity(&m);
  return m;
}

void
mat4_set_identity(Matrix4* m)
{
  m->m[0] = 1;
  m->m[5] = 1;
  m->m[10] = 1;
  m->m[15] = 1;
  m->m[1] = m->m[2] = m->m[3] = m->m[4] = m->m[6] = m->m[7] = 0;
  m->m[8] = m->m[9] = m->m[11] = m->m[12] = m->m[13] = m->m[14] = 0;
}

Matrix4
mat4_identity()
{
  Matrix4 m;
  //printf("before : %f \n", m.m[6]);
  mat4_set_identity(&m);
  //printf("after : %f \n", m.m[6]);
  return m;
}

Matrix3GL
mat3_to_gl(Matrix3 m)
{
  Matrix3GL glm;
  int i;

  for (i = 0; i < 9; ++i) {
    glm.m[i] = (GLfloat) m.m[i];
  }

  return glm;
}

Matrix4GL
mat4_to_gl(Matrix4 m)
{
  Matrix4GL glm;
  int i;

  for (i = 0; i < 16; ++i) {
    glm.m[i] = (GLfloat) m.m[i];
  }

  return glm;
}

Matrix4 
mat4_frustum(
      double left,
      double right,
      double bottom,
      double top,
      double near,
      double far)
{
  Matrix4 m = mat4();

  int i = 0;
  for (i = 0; i < 16; ++i) {
    if (m.m[i] != 0)
    printf("hannnnnnnnnnn matrix is not init to zero m[%d]=%f\n", i, m.m[i]);
  }

  m.m[0]  =  2 * near / (right - left);
  m.m[2]  =  (right + left) / (right - left);
  m.m[5]  =  2 * near / (top - bottom);
  m.m[6]  =  (top + bottom) / (top - bottom);
  m.m[10] = -(far + near) / (far - near);
  m.m[11] = -(2 * far * near) / (far - near);
  m.m[14] = -1;
  m.m[15] =  0;

  return m;
}

Matrix4
mat4_multiply(Matrix4 m, Matrix4 n)
{
  Matrix4 newmat;
  newmat.m[0] = m.m[0]*n.m[0] + m.m[1]*n.m[4] + m.m[2]*n.m[8] + m.m[3]*n.m[12];
  newmat.m[1] = m.m[0]*n.m[1] + m.m[1]*n.m[5] + m.m[2]*n.m[9] + m.m[3]*n.m[13];
  newmat.m[2] = m.m[0]*n.m[2] + m.m[1]*n.m[6] + m.m[2]*n.m[10] + m.m[3]*n.m[14];
  newmat.m[3] = m.m[0]*n.m[3] + m.m[1]*n.m[7] + m.m[2]*n.m[11] + m.m[3]*n.m[15];
  newmat.m[4] = m.m[4]*n.m[0] + m.m[5]*n.m[4] + m.m[6]*n.m[8] + m.m[7]*n.m[12];
  newmat.m[5] = m.m[4]*n.m[1] + m.m[5]*n.m[5] + m.m[6]*n.m[9] + m.m[7]*n.m[13];
  newmat.m[6] = m.m[4]*n.m[2] + m.m[5]*n.m[6] + m.m[6]*n.m[10] + m.m[7]*n.m[14];
  newmat.m[7] = m.m[4]*n.m[3] + m.m[5]*n.m[7] + m.m[6]*n.m[11] + m.m[7]*n.m[15];
  newmat.m[8] = m.m[8]*n.m[0] + m.m[9]*n.m[4] + m.m[10]*n.m[8] + m.m[11]*n.m[12];
  newmat.m[9] = m.m[8]*n.m[1] + m.m[9]*n.m[5] + m.m[10]*n.m[9] + m.m[11]*n.m[13];
  newmat.m[10] = m.m[8]*n.m[2] + m.m[9]*n.m[6] + m.m[10]*n.m[10] + m.m[11]*n.m[14];
  newmat.m[11] = m.m[8]*n.m[3] + m.m[9]*n.m[7] + m.m[10]*n.m[11] + m.m[11]*n.m[15];
  newmat.m[12] = m.m[12]*n.m[0] + m.m[13]*n.m[4] + m.m[14]*n.m[8] + m.m[15]*n.m[12];
  newmat.m[13] = m.m[12]*n.m[1] + m.m[13]*n.m[5] + m.m[14]*n.m[9] + m.m[15]*n.m[13];
  newmat.m[14] = m.m[12]*n.m[2] + m.m[13]*n.m[6] + m.m[14]*n.m[10] + m.m[15]*n.m[14];
  newmat.m[15] = m.m[12]*n.m[3] + m.m[13]*n.m[7] + m.m[14]*n.m[11] + m.m[15]*n.m[15];

  return newmat;
}

Matrix4
mat4_transpose(Matrix4 m)
{
  Matrix4 tm;

  tm.m[0] = m.m[0];
  tm.m[1] = m.m[4];
  tm.m[2] = m.m[8];
  tm.m[3] = m.m[12];

  tm.m[4] = m.m[1];
  tm.m[5] = m.m[5];
  tm.m[6] = m.m[9];
  tm.m[7] = m.m[13];

  tm.m[8] = m.m[2];
  tm.m[9] = m.m[6];
  tm.m[10]= m.m[10];
  tm.m[11]= m.m[14];

  tm.m[12]= m.m[3];
  tm.m[13]= m.m[7];
  tm.m[14]= m.m[11];
  tm.m[15]= m.m[15];

  return tm;
}

Matrix4 
mat4_translation(double x, double y, double z)
{
  Matrix4 m;
  m.m[0] = m.m[5] = m.m[10] = m.m[15] = 1;
  m.m[3] = x;
  m.m[7] = y;
  m.m[11] = z;

  m.m[1] = m.m[2] = m.m[4] = 0;
  m.m[6] = m.m[8] = m.m[9] = 0;
  m.m[12] = m.m[13] = m.m[14] = 0;
  //mat4_set_identity(&m);
  return m;
}

Matrix4
mat4()
{
  Matrix4 m;
  int i;
  for (i = 0; i < 16; ++i) m.m[i] = 0;
  return m;
}

Matrix3
mat3()
{
  Matrix3 m;
  int i;
  for (i = 0; i < 9; ++i) m.m[i] = 0;
  return m;
}

