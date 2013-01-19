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
  Vec3 position_base;
  Quat rotation;
  Quat rotation_base;
  Eina_List* children;
};

typedef struct _Frame Frame;
struct _Frame
{
  float time;
  union{
    Vec3 vec3;
    Vec4 vec4;
    Quat quat;
  };
};
//TODO ou alors je fais un truc avec 
// frame avec quaternion, position and scale...?


typedef enum _DataType
{
  POSITION,
  QUATERNION,
  EULER,
  SCALE

} DataType;

typedef struct _Curve Curve;
struct _Curve
{
  Bone* bone; //or just name?
  DataType type;
  Eina_Inarray* frames;

};


typedef struct _Action Action;
struct _Action
{
  char* name;
  Eina_List* curves; //could be 3 curves : position, quaternion, scale
};


typedef struct _Armature Armature;
struct _Armature
{
  char* name;
  Vec3 position;
  Quat rotation;
  Eina_List* actions;
  Eina_List* bones;
};

void armature_read_file(Armature* armature, FILE* f);
Armature* create_armature_file(FILE* f);
void armature_add_bone(Armature* a, Bone* b);
void armature_add_action(Armature* a, Action* action);
void bone_add_child(Bone* b, Bone* child);
Action* armature_find_action(Armature* armature, char* action_name);

Frame* curve_find_frame(Curve* curve, float time);

#endif
