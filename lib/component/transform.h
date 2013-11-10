#ifndef __component_transform__
#define __component_transform__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component.h"

typedef struct _Transform Transform;

struct _Transform
{
  Vec3 position;
  Vec3 angles;
  double test;
  Quat orientation;
  Matrix4 matrix; //computed from position and orientation
};

void test_test();

ComponentDesc transform_desc;


#endif
