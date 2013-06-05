#ifndef __component_transform__
#define __component_transform__
#include "vec.h"

typedef struct
{
  Vec3 position;
  Vec3 angles;
  double test;

} Transform;

void test_test();

typedef struct
{
  const char* name;
  int type;
} Prop;

#endif
