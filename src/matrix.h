#ifndef __matrix__
#define __matrix__
//#include "Elementary.h"
#include "GL/gl.h"

/*
typedef double Matrix4[16];
typedef double Matrix3[9];
typedef GLfloat Matrix4GL[16];
typedef GLfloat Matrix3GL[9];
*/

typedef struct _Matrix4 Matrix4;

struct _Matrix4 {
  double m[16];
};

typedef struct _Matrix3 Matrix3;

struct _Matrix3 {
  double m[9];
};

typedef struct _Matrix3GL Matrix3GL;

struct _Matrix3GL {
  GLfloat m[9];
};

typedef struct _Matrix4GL Matrix4GL;

struct _Matrix4GL {
  GLfloat m[16];
};


Matrix3 mat4_to_mat3(Matrix4 m);
Matrix3 mat3_inverse(Matrix3 m);

Matrix3 mat3_identity();
Matrix4 mat4_identity();

void mat3_set_identity(Matrix3* m);
void mat4_set_identity(Matrix4* m);

Matrix3GL mat3_to_gl(Matrix3 m);
Matrix4GL mat4_to_gl(Matrix4 m);

Matrix4 mat4_frustum(
      double left,
      double right,
      double bottom,
      double top,
      double near,
      double far);

Matrix4 mat4_multiply(Matrix4 m, Matrix4 n);

Matrix4 mat4_transpose(Matrix4 m);

Matrix4 mat4_translation(double x, double y, double z);
Matrix4 mat4();
Matrix3 mat3();

#endif


