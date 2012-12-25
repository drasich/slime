#ifndef __matrix__
#define __matrix__
//#include "GL/gl.h"
#include "Evas_GL.h"
#include "quat.h"

typedef double Matrix4[16];
typedef double Matrix3[9];
typedef GLfloat Matrix4GL[16];
typedef GLfloat Matrix3GL[9];

void mat4_to_mat3(Matrix4 in, Matrix3 out);
void mat3_inverse(Matrix3 in, Matrix3 out);

void mat3_set_identity(Matrix3 m);
void mat4_set_identity(Matrix4 m);

void mat3_to_gl(Matrix3 in, Matrix3GL out);
void mat4_to_gl(Matrix4 in, Matrix4GL out);

void mat4_set_frustum(
      Matrix4 m,
      double left,
      double right,
      double bottom,
      double top,
      double near,
      double far);

void mat4_multiply(Matrix4 m, Matrix4 n, Matrix4 out);

void mat4_transpose(Matrix4 in, Matrix4 out);

void mat4_set_translation(Matrix4 m, Vec3);

void mat4_zero(Matrix4 m);
void mat3_zero(Matrix4 m);

void mat4_set_rotation_quat(Matrix4 m, Quat q);

#endif


