#ifndef __control__
#define __control__
#include <Elementary.h>
#include "vec.h"
#include "object.h"

typedef struct _Control Control;

enum {
  IDLE,
  MOVE
};

struct _Control{
  struct _View* view;
  int state;
  Vec3 start;
  Eina_List* undo;
  Eina_List* redo;
};

Control* create_control(struct _View* v);
void control_set_state(Control* c, int state);
void control_move(Control* c);

void control_mouse_move(Control* c, Evas_Event_Mouse_Move *e);
void control_mouse_down(Control* c, Evas_Event_Mouse_Down *e);
void control_key_down(Control* c, Evas_Event_Key_Down *e);

typedef void (*do_cb)(void* data);
typedef void (*undo_cb)(void* data);

typedef struct _Operation Operation;
struct _Operation {
  do_cb do_cb;
  undo_cb undo_cb;
  void* data;
};

typedef struct _Op_Move_Object Op_Move_Object;
struct _Op_Move_Object
{
  Object* o;
  Vec3 start;
  Vec3 end;
};

void operation_move_object_do(void* data);
void operation_move_object_undo(void* data);

void control_add_operation(Control* c, Operation* op);
void control_undo(Control* c);
void control_redo(Control* c);

void control_clean_redo(Control* c);

#endif
