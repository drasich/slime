#ifndef __armature__
#define __armature__
#include "vec.h"
#include "quat.h"
#include <Eina.h>

typedef struct _Bone Bone;
struct _Bone
{
  char* name;
  Vec3 position;
  Quat rotation;
  Eina_List* children;
};

typedef struct _Armature Armature;
struct _Armature
{
  char* name;
  Vec3 position;
  Quat rotation;
  Eina_List* bones;
};

void armature_read_file(Armature* armature, FILE* f);
Armature* create_armature_file(FILE* f);
void armature_add_bone(Armature* a, Bone* b);
void bone_add_child(Bone* b, Bone* child);


#endif
