#include "animation.h"
#include "object.h"

void
animation_play(Object* o, char* name, PlayMode mode)
{
  Animation* a = o->animation;
  if (a == NULL) return;

  a->action_current = armature_find_action(o->armature, name);
  a->current = name;
  a->mode = mode;
  a->speed = 1;
  a->status = PLAY;
  a->time = a->action_current->frame_start / 30.0f;
  object_set_pose(o, a->current, a->time);
}

void
animation_stop(Object* o)
{
  Animation* a = o->animation;
  if (a == NULL) return;
  if (a->status != PLAY && a->status != PAUSE) return;

  a->status = STOP;
  a->time = a->action_current->frame_start / 30.0f;
  object_set_pose(o, a->current, a->time);
}

void
animation_pause(Object* o)
{
  Animation* a = o->animation;
  if (a == NULL) return;
  if (a->status != PLAY) return;

  a->status = PAUSE;
}

void
animation_resume(Object* o)
{
  Animation* a = o->animation;
  if (a == NULL) return;
  if (a->status != PAUSE) return;

  a->status = PLAY;
}



