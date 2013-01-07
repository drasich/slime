#ifndef __armature__
#define __armature__
#include "matrix.h"
#include <stdbool.h>

typedef struct _Bone Bone;
struct _Bone
{
  char* name;
  Vec3 position;
  Quat rotation;
  //Quat rotation;
  //list of children...
};

typedef struct _Armature Armature;
struct _Armature
{
  char* name;
  //Vec3 position;
  //Quat rotation;
  //list of bones...
  //tree of bones?
};



#endif
