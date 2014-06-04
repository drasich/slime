#include "animation.h"
#include "object.h"

void
animation_play(Object* o, char* name, PlayMode mode)
{
  Animation* a = object_animation_get(o);
  Armature* armature = object_armature_get(o);
  if (a == NULL || !armature) return;

  a->action_current = armature_find_action(armature, name);
  if (!a->action_current) {
    return;
  }
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
  Animation* a = object_animation_get(o);
  if (a == NULL) return;
  if (a->status != PLAY && a->status != PAUSE) return;

  a->status = STOP;
  a->time = a->action_current->frame_start / 30.0f;
  object_set_pose(o, a->current, a->time);
}

void
animation_pause(Object* o)
{
  Animation* a = object_animation_get(o);
  if (a == NULL) return;
  if (a->status != PLAY) return;

  a->status = PAUSE;
}

void
animation_resume(Object* o)
{
  Animation* a = object_animation_get(o);
  if (a == NULL) return;
  if (a->status != PAUSE) return;

  a->status = PLAY;
}

Eina_List*
animation_list(struct _Object* o)
{
  Armature* armature = object_armature_get(o);
  if (!armature) {
    EINA_LOG_WARN("object '%s' has no armature", o->name);
    return NULL;
  }

  Eina_List* strings = NULL;

  Eina_List* l;
  Action* action;
  EINA_LIST_FOREACH(armature->actions, l, action) {
    strings = eina_list_append(strings, action->name);
  }

  return strings;
}


