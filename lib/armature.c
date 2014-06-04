#include "armature.h"
#include "read.h"
#include <float.h>
#include "log.h"

Armature*
armature_new()
{
  Armature* a = calloc(1,sizeof *a);
  return a;
}

Armature*
armature_file_set(Armature* a, const char *filename)
{
  a->bones = NULL;
  a->actions = NULL;

  FILE *f;
  f = fopen(filename, "rb");
  if (!f) {
    EINA_LOG_DOM_ERR(log_mesh_dom, "cannot open file '%s'", filename);
  }
  fseek(f, 0, SEEK_SET);

  const char* type = read_string(f);

  armature_read_file(a, f);
  return a;
}


Armature*
create_armature_file(FILE* f)
{
  Armature* a = calloc(1,sizeof(Armature));
  a->bones = NULL;
  a->actions = NULL;
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
  //printf("bone name '%s'\n",bone->name);
  bone->position_base = read_vec3(f);
  bone->rotation_base = read_vec4(f);
  Quat q = bone->rotation_base;
  //printf("  bone rotation : %f, %f, %f, %f\n", q.x, q.y, q.z, q.w);
  bone->position = bone->position_base;
  bone->rotation = bone->rotation_base;

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
  curve->frame_start = FLT_MAX ;
  curve->frame_end = -FLT_MAX;

  char* bone_name = read_name(f);
  curve->bone = armature_find_bone(armature, bone_name);

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
  curve->frames = eina_inarray_new(sizeof(Frame), frames_nb);
  int i;
  for (i = 0; i < frames_nb; ++i) {
    Frame frame;
    frame.time = read_float(f);
    //printf("curve time : %f ", frame.time);
    if (frame.time < curve->frame_start) curve->frame_start = frame.time;
    if (frame.time > curve->frame_end) curve->frame_end = frame.time;
    if (curve->type == QUATERNION) {
      frame.quat = read_vec4(f);
    } else {
      frame.vec3 = read_vec3(f);
    }
    eina_inarray_push(curve->frames, &frame);
  }
  
  return curve;
}

void
action_add_curve(Action* a, Curve* c)
{
  a->curves = eina_list_append(a->curves,c);
  if (c->frame_start < a->frame_start) a->frame_start = c->frame_start;
  if (c->frame_end > a->frame_end) a->frame_end = c->frame_end;
}

Action*
action_create(FILE* f, Armature* armature)
{
  Action* action = malloc(sizeof(Action));
  action->curves = NULL;
  action->name = read_name(f);
  printf("action name: %s\n", action->name);

  action->frame_start = FLT_MAX;
  action->frame_end = -FLT_MAX;

  uint16_t curves_nb = read_uint16(f);
  int i;
  printf("curves count: %d\n", curves_nb);
  for (i = 0; i < curves_nb; ++i) {
    Curve* c = curve_create(f, armature);
    action_add_curve(action,c);
  }

  return action;
}

void
armature_read_file(Armature* armature, FILE* f)
{
  printf("armature_read-file\n");
  armature->name = read_name(f);
  printf("armature name: %s\n", armature->name);

  armature->position = read_vec3(f);
  armature->rotation = read_vec4(f);
  armature->scale = read_vec3(f);

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
    armature_add_action(armature, a);
  }
}

void
armature_add_bone(Armature* a, Bone* b)
{
   a->bones = eina_list_append(a->bones, b);
}

void
armature_add_action(Armature* armature, Action* action)
{
   armature->actions = eina_list_append(armature->actions, action);
}

void
bone_add_child(Bone* b, Bone* child)
{
   b->children = eina_list_append(b->children, child);
}

Action*
armature_find_action(Armature* armature, char* action_name)
{
   Eina_List *l;
   Action *action;
   EINA_LIST_FOREACH(armature->actions, l, action) {
     if (!strcmp(action->name, action_name)) {
       return action;
     }
   }

  return NULL;
}

Frame*
curve_find_frame(Curve* curve, float time)
{
  //TODO can optimize this by looking in the middle
  Frame* fr;
  EINA_INARRAY_FOREACH(curve->frames, fr) {
    if (time <= fr->time) return fr;
  }
  return NULL;
}

void
curve_get_frames(Curve* curve, float time, Frame** start, Frame** end)
{
  //TODO can optimize this by looking in the middle
  Frame* f;
  *start = NULL;
  
  EINA_INARRAY_FOREACH(curve->frames, f) {
    if (time == f->time){
      *start = f ;
      *end = f;
      return;
    }
    else if (time > f->time) {
      *start = f;
    }
    else if (time < f->time) {
      *end = f;
      return;
    }
  }

  if (*start == NULL) *start = f;
  *end = f;
}

void
armature_set_pose(Armature* armature, char* action_name, float time)
{
  Action* action = armature_find_action(armature, action_name);

  int frame = time *30;
  if (frame < action->frame_start) frame = action->frame_start;
  if (frame > action->frame_end) frame = action->frame_end;

  Eina_List *l;
  Curve *curve;
  EINA_LIST_FOREACH(action->curves, l, curve) {
    Bone* bone = curve->bone;
    Frame *start, *end;
    curve_get_frames(curve, frame, &start, &end);

    float ratio = 0;
    if (start != end)
    ratio = (frame - start->time) / (end->time - start->time);

    if (curve->type == POSITION) {
      Vec3 v1 = vec3_mul(start->vec3, 1-ratio);
      Vec3 v2 = vec3_mul(end->vec3, ratio);
      bone->position = vec3_add(v1, v2);
    } else if (curve->type == QUATERNION) {
      bone->rotation = quat_slerp(start->quat, end->quat, ratio);
    } else if (curve->type == EULER) {
      /*
      Vec3 euler = f->vec3;
      printf("euler %f %f %f\n", euler.x, euler.y, euler.z);
      Vec3 axisz = {0,0,1};
      Quat qz = quat_angle_axis(euler.z, axisz);
      Vec3 axisy = {0,1,0};
      Quat qy = quat_angle_axis(euler.y, axisy);
      Vec3 axisx = {1,0,0};
      Quat qx = quat_angle_axis(euler.x, axisx);
      Quat q = quat_mul( quat_mul (qz, qy), qx);
      //Quat q = quat_mul( quat_mul (qx, qy), qz);

      //bone->rotation = q;
      */
    }

  }
}

