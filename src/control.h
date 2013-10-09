#ifndef __control__
#define __control__
#include <Elementary.h>
#include "vec.h"
#include "scene.h"
//#include "object.h"
#include "property.h"
#include "component.h"

typedef struct _Control Control;

enum {
  CONTROL_IDLE,
  CONTROL_MOVE,
  CONTROL_SCALE,
  CONTROL_DRAGGER
};

struct _Control{
  struct _View* view;
  int state;
  Vec3 start;
  Vec2 mouse_start;
  Vec3 mouse_current;
  Vec3 constraint;
  Eina_Inarray* positions;
  Eina_List* undo;
  Eina_List* redo;

  Eina_Inarray* scales;
  float scale_factor;
};


Control* create_control(struct _View* v);

void control_mouse_move(Control* c, Evas_Event_Mouse_Move *e);
bool control_mouse_down(Control* c, Evas_Event_Mouse_Down *e);
bool control_mouse_up(Control* c, Evas_Event_Mouse_Up *e);
void control_key_down(Control* c, Evas_Event_Key_Down *e);

void control_property_update(Control* c, Component* component);

typedef void (*do_cb)(Control* c, void* data);
typedef void (*undo_cb)(Control* c, void* data);

typedef struct _Operation Operation;
struct _Operation {
  do_cb do_cb;
  undo_cb undo_cb;
  void* data;
};


void control_operation_add(Control* c, Operation* op);
void control_undo(Control* c);
void control_redo(Control* c);
void control_redo_clean(Control* c);

void control_object_add(Control* c, Scene* s, Object* o);
void control_property_change(Control* c, Component* component, Property* p, const void* data_old, const void* data_new);
void control_component_add(Control* c, Object* o, Component* comp);
void control_component_remove(Control* c, Object* o, Component* comp);

void control_property_transform_update(Control* c);


//events
void control_on_object_components_changed(Control* c, Object* o);

#endif
