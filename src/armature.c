#include "armature.h"
#include "read.h"

Armature*
create_armature_file(FILE* f)
{
  Armature* a = calloc(1,sizeof(Armature));
  a->bones = NULL;
  eina_init();
  armature_read_file(a, f);
  return a;
}

Bone*
bone_create(FILE* f)
{
  Bone* bone = malloc(sizeof(Bone));
  bone->children = NULL;
  bone->name = read_name(f);
  printf("bone name '%s'\n",bone->name);
  bone->position = read_vec3(f);
  bone->rotation = read_vec4(f);

  uint16_t child_nb = read_uint16(f);
  int i;
  for (i = 0; i < child_nb; ++i) {
    Bone* c = bone_create(f);
    bone_add_child(bone,c);
  }

  return bone;
}

Bone*
bone_find_child(Bone* bone, char* child_name)
{
  Eina_List *children;
  Bone *child;
  EINA_LIST_FOREACH(bone->children, children, child) {
     if (!strcmp(child->name, child_name)){
       return child;
     } else {
        Bone* b = bone_find_child(child, child_name);
        if (b) return b;
     }
  }

  return NULL;
}

Bone*
armature_find_bone(Armature* a, char* bone_name)
{
   Eina_List *l;
   Bone *bone;
   EINA_LIST_FOREACH(a->bones, l, bone) {
     if (!strcmp(bone->name, bone_name)) {
       return bone;
     } else {
       Bone* b = bone_find_child(bone, bone_name);
       if (b) return b;
     }
   }
  
  return NULL;
}

Curve*
curve_create(FILE* f, Armature* armature)
{
  Curve* curve = malloc(sizeof(Curve));
  curve->frames = NULL;

  char* bone_name = read_name(f);
  printf("curve, bone bane : %s\n", bone_name);
  curve->bone = armature_find_bone(armature, bone_name);
  if (curve->bone)
    printf("curve, bone  found bane : %s\n", curve->bone->name);

  char* type = read_name(f);

  if (!strcmp(type, "position")) {
    curve->type = POSITION;
  } else if (!strcmp(type, "quaternion")) {
    curve->type = QUATERNION;
  } else if (!strcmp(type, "euler")) {
    curve->type = EULER;
  } else if (!strcmp(type, "scale")) {
    curve->type = SCALE;
  }

  //TODO add the frame to the curve
  uint16_t frames_nb = read_uint16(f);
  int i;
  for (i = 0; i < frames_nb; ++i) {
    Frame frame;
    frame.time = read_float(f);
    if (curve->type == QUATERNION) {
      frame.quat = read_vec4(f);
    } else {
      frame.vec3 = read_vec3(f);
    }
  }

  return curve;
}

Action*
action_create(FILE* f, Armature* armature)
{
  Action* action = malloc(sizeof(Action));
  action->curves = NULL;
  action->name = read_name(f);
  printf("action name: %s\n", action->name);

  uint16_t curves_nb = read_uint16(f);
  int i;
  printf("curves count: %d\n", curves_nb);
  for (i = 0; i < curves_nb; ++i) {
    Curve* c = curve_create(f, armature);
    //TODO
    //action_add_curve(action,c);
  }

  return action;
}

void
armature_read_file(Armature* armature, FILE* f)
{
  printf("armature_read-file\n");
  armature->name = read_name(f);
  printf("armature name: %s\n", armature->name);

  uint16_t bone_count;
  fread(&bone_count, sizeof(bone_count),1,f);
  printf("bone count: %d\n", bone_count);

  int i;
  for (i = 0; i < bone_count; ++i) {
    Bone* b = bone_create(f);
    armature_add_bone(armature, b);
  }

  uint16_t action_count = read_uint16(f);
  printf("action count: %d\n", action_count);
  for (i = 0; i < action_count; ++i) {
    Action* a = action_create(f, armature);
    //TODO
  }
}

void
armature_add_bone(Armature* a, Bone* b)
{
   a->bones = eina_list_append(a->bones, b);
}

void
bone_add_child(Bone* b, Bone* child)
{
   b->children = eina_list_append(b->children, child);
}
