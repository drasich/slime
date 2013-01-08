#include "armature.h"
#include "read.h"

Armature* create_armature_file(FILE* f)
{
  Armature* a = calloc(1,sizeof(Armature));
  a->bones = NULL;
  eina_init();
  armature_read_file(a, f);
  return a;
}

Bone* bone_create( FILE* f)
{
  Bone* bone = malloc(sizeof(Bone));
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

void armature_read_file(Armature* armature, FILE* f)
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
}

void armature_add_bone(Armature* a, Bone* b)
{
   a->bones = eina_list_append(a->bones, b);
}

void bone_add_child(Bone* b, Bone* child)
{
   b->children = eina_list_append(b->children, child);
}
