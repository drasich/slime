#ifndef __control__
#define __control__
#include <Elementary.h>
#include "vec.h"
#include "scene.h"
//#include "object.h"
#include "property.h"

typedef struct _Control Control;

enum {
  IDLE,
  MOVE
};

struct _Control{
  struct _View* view;
  int state;
  Vec3 start;
  Vec2 mouse_start;
  Vec3 mouse_current;
  Eina_Inarray* positions;
  Eina_List* undo;
  Eina_List* redo;
  Shader* shader_simple;//TODO remove from here
};


Control* create_control(struct _View* v);

void control_mouse_move(Control* c, Evas_Event_Mouse_Move *e);
bool control_mouse_down(Control* c, Evas_Event_Mouse_Down *e);
void control_key_down(Control* c, Evas_Event_Key_Down *e);

void control_property_changed(Control* c, void* data, Property* p);

typedef void (*do_cb)(Control* c, void* data);
typedef void (*undo_cb)(Control* c, void* data);

typedef struct _Operation Operation;
struct _Operation {
  do_cb do_cb;
  undo_cb undo_cb;
  void* data;
};


void control_add_operation(Control* c, Operation* op);
void control_undo(Control* c);
void control_redo(Control* c);
void control_clean_redo(Control* c);

#endif
