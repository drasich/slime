#ifndef __control__
#define __control__
#include <Elementary.h>
#include "vec.h"

typedef struct _Control Control;

enum {
  IDLE,
  MOVE
};

struct _Control{
  struct _View* view;
  int state;
  Vec3 start;
};

Control* create_control(struct _View* v);
void control_set_state(Control* c, int state);
void control_move(Control* c);

void control_mouse_move(Control* c, Evas_Event_Mouse_Move *e);
void control_mouse_down(Control* c, Evas_Event_Mouse_Down *e);
void control_key_down(Control* c, Evas_Event_Key_Down *e);


#endif
