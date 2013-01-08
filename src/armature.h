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
  Eina_List* children;
};

typedef struct _Armature Armature;
struct _Armature
{
  char* name;
  //Vec3 position;
  //Quat rotation;
  //list of bones...
  //tree of bones?
  Eina_List* bones;
};

void armature_read_file(Armature* armature, FILE* f);
Armature* create_armature_file(FILE* f);
void armature_add_bone(Armature* a, Bone* b);
void bone_add_child(Bone* b, Bone* child);


#endif
