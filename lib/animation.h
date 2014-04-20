#ifndef __animation__
#define __animation__
#include <stdbool.h>
#include "Eina.h"

typedef enum _PlayMode
{
  ONCE,
  ONCE_REVERSE,
  LOOP,
  LOOP_REVERSE,
  PING_PONG
} PlayMode;

typedef enum _PlayStatus
{
  NONE,
  STOP,
  PLAY,
  PAUSE,
  FINISHED
} PlayStatus;

typedef struct _Animation Animation;
struct _Animation
{
  struct _Object* object;
  char* current;
  struct _Action* action_current;
  PlayMode mode;
  PlayStatus status;
  float time;
  float speed;
};

void animation_play(struct _Object* o, char* name, PlayMode mode);
void animation_pause(struct _Object* o);
void animation_resume(struct _Object* o);
void animation_stop(struct _Object* o);
Eina_List* animation_list(struct _Object* o);

//TODO
//void animation_crossfade(Object* o, char* name, float time, PlayMode mode);
// additive animations


#endif
