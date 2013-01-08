#include "armature.h"

Armature* create_armature_file(FILE* f)
{
   Armature* a = calloc(1,sizeof(Armature));
   armature_read_file(a, f);
   return a;
}

char* object_read_string(FILE* f)
{
  uint16_t strlen;
  fread(&strlen, sizeof(strlen),1,f);
  printf("read string strlen: %d\n", strlen);
  char* str = malloc(strlen+1);
  fread(str, 1, strlen, f);
  str[strlen] = '\0';
  printf("read string: %s\n", str);
  return str;
}

Bone* bone_create( FILE* f)
{
    Bone* bone = malloc(sizeof(Armature));
    //name
    //position
    //quaternion
    // number of childs
    // for number of childs
    //  Bone* c =  bone_create(f);
    //  bone_add_child(bone,c);

    return bone;
  
}


void armature_read_file(Armature* armature, FILE* f)
{
  printf("armature_read-file\n");
  // read name
  uint16_t strlen;
  fread(&strlen, sizeof(strlen),1,f);
  printf("strlen: %d\n", strlen);
  char* name = malloc(strlen+1);
  fread(name, 1, strlen, f);
  name[strlen] = '\0';
  printf("name: %s\n", name);
  // read name

  uint16_t bone_count;
  fread(&bone_count, sizeof(bone_count),1,f);
  printf("bone count: %d\n", bone_count);

  int i;
  for (i = 0; i < bone_count; ++i) {
    Bone* b = bone_create(f);
    //armature_add_bone(armature, b);
  }

}

